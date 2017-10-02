#pragma once
#include "math3d.hpp"
#include "helpers.hpp"

class Renderer
{
public:
	Renderer();
	~Renderer();

	struct ConstantBuffer
	{
		mat4 WVP; // World View Projection
		vec3 lightPos;
		vec3 viewPos;
	};

	void resizeRenderTarget(int width, int height)
	{
		width_ = width;
		height_ = height;
		zbuffer.resize(width * height);
	}

	template <typename T, class RenderTarget>
	void render(const std::vector<T>& vb, const std::vector<int>& ib, const ConstantBuffer& cb, RenderTarget& render_target)
	{
		cb_ = cb;
		IaStage(ib);
		VSStage(vb);
		EarlyZ();
		PSStage(render_target);
	}

protected:
	struct polygon_
	{
		int indices[3];
	};

	struct vertex_output
	{
		vec4 pos;
		vec2 uv;
	};

	void Clear();
	void IaStage(const std::vector<int>& ib);

	DEF_HAS_MEMBER(uv)

	template <typename T>
	void VSStage(const std::vector<T>& vb)
	{
		vb_after_vs_.resize(vb.size());

		for (int i = 0; i < vb.size(); ++i)
		{
			vec4 origin_pos = vb[i].pos;
			origin_pos.data()[3] = 1;
			vb_after_vs_[i].pos = origin_pos * cb_.WVP;
			//				vb_after_vs_[i].depthPos = origin_pos * cb_.WVP;

			// ¹éÒ»»¯
			vb_after_vs_[i].pos = vb_after_vs_[i].pos / vb_after_vs_[i].pos.data()[3];
//			vb_after_vs_[i].pos.data()[0] = vb_after_vs_[i].pos.data()[0] / vb_after_vs_[i].pos.data()[3];
//			vb_after_vs_[i].pos.data()[1] = vb_after_vs_[i].pos.data()[1] / vb_after_vs_[i].pos.data()[3];
//			vb_after_vs_[i].pos.data()[2] = vb_after_vs_[i].pos.data()[2] / vb_after_vs_[i].pos.data()[3];

			if constexpr(HAS_MEMBER(T, uv))
			{
				vb_after_vs_[i].uv = vb[i].uv;
			}
		}
	}
	void EarlyZ();

	template <class RenderTarget>
	void PSStage(RenderTarget& render_target)
	{
		class PS
		{
		public:
			static void shading(vertex_output& x)
			{
			}
		};

		for (int i = 0; i < triangles_.size(); ++i)
		{
			RasterizeTriangle<PS>(i, render_target);
		}
	}



	template <typename PixelShader, class RenderTarget>
	void RasterizeTriangle(int index, RenderTarget& render_target)
	{
		auto& tri = triangles_[index];
		vertex_output vertices[3];
		vec2 screen_cords[3];
		for (int i = 0; i < 3; ++i)
		{
			vertices[i] = vb_after_vs_[tri.indices[i]];
			screen_cords[i].data()[0] = (vertices[i].pos.data()[0] + 1) / 2 * width_;
			screen_cords[i].data()[1] = (vertices[i].pos.data()[1] + 1) / 2 * height_;
		}
		struct Rect
		{
			float bottom, top, left, right;
		};

		Rect rect;
		rect.bottom = 0;
		rect.top = height_;
		rect.left = width_;
		rect.right = 0;

		for (int i = 0; i < 3; ++i)
		{
			if (screen_cords[i].data()[0] < rect.left) rect.left = screen_cords[i].data()[0];
			if (screen_cords[i].data()[0] > rect.right) rect.right = screen_cords[i].data()[0];
			if (screen_cords[i].data()[1] < rect.top) rect.top = screen_cords[i].data()[1];
			if (screen_cords[i].data()[1] > rect.bottom) rect.bottom = screen_cords[i].data()[1];
		}

		rect.bottom = min(rect.bottom, height_);
		rect.top = max(rect.top, 0);
		rect.left = min(rect.left, width_);
		rect.right = max(rect.right, 0);

		//	static std::vector<uint8_t> in_line;
		//	in_line.resize(height_);
		//	std::vector<uint8_t> out_line;

		const auto& x1 = screen_cords[0].data()[0];
		const auto& x2 = screen_cords[1].data()[0];
		const auto& x3 = screen_cords[2].data()[0];
		const auto& y1 = screen_cords[0].data()[1];
		const auto& y2 = screen_cords[0].data()[1];
		const auto& y3 = screen_cords[0].data()[1];

		auto denominator = (y2 - y3) * (x1 - x3) + (x3 - x2) * (y1 - y3);
		auto numerator1_1 = (y2 - y3) / denominator;
		auto numerator1_2 = (x3 - x2) / denominator;
		auto numerator2_1 = (y3 - x1) / denominator;
		auto numerator2_2 = (x1 - x3) / denominator;

		for (int y = rect.top; y < rect.bottom; ++y)
		{
			bool in_line = false;
			for (int x = rect.left; x < rect.right; ++x)
			{
				auto lambda1 = numerator1_1 * (x - x3) + numerator1_2 * (y - y3);
				auto lambda2 = numerator2_1 * (x - x3) + numerator2_2 * (y - y3);
				auto lambda3 = 1 - lambda1 - lambda2;
				const bool is_in = lambda1 < 1 && lambda1 > DBL_EPSILON && lambda2 < 1 && lambda2 > DBL_EPSILON;
				if (is_in)
				{
					// do
					vertex_output interpolated;
					interpolated.pos = lambda1 * vertices[0].pos + lambda2 * vertices[1].pos + lambda3 * vertices[2].pos;
					auto zr =
						lambda1 / vertices[0].pos.data()[2] +
						lambda2 / vertices[1].pos.data()[2] +
						lambda3 / vertices[2].pos.data()[2];
					auto lambda1_c = lambda1 / vertices[0].pos.data()[2] / zr;
					auto lambda2_c = lambda2 / vertices[1].pos.data()[2] / zr;
					auto lambda3_c = lambda3 / vertices[2].pos.data()[2] / zr;

					// Z-PASS
					if (interpolated.pos.data()[2] < zbuffer[y * height_ + x])
					{
						zbuffer[y * height_ + x] = interpolated.pos.data()[2];

						interpolated.uv = lambda1_c * vertices[0].uv + lambda2_c * vertices[1].uv + lambda3_c * vertices[2].uv;
						if constexpr (!std::is_void<decltype(PixelShader::shading(interpolated))>::value)
						{
							auto ret = PixelShader::shading(interpolated);
							render_target.DrawPoint(x, y, ret);
						}
					}

					if (!in_line)
						in_line = true;
				}
				else if (in_line)
				{
					break;// finish line
				}
			}
		}
	}


	ConstantBuffer cb_;
	std::vector<polygon_> triangles_;
	std::vector<vertex_output> vb_after_vs_;
	std::vector<float> zbuffer;
	int width_;
	int height_;
};
