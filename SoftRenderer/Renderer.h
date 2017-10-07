#pragma once
#include "math3d.hpp"
#include "helpers.hpp"
#include <vector>

static int a = 0xFF000000;


class Renderer
{
public:
	Renderer();
	~Renderer();

	struct ConstantBuffer
	{
		mat4 WVP; // World View Projection
		mat4 view; // World View Projection
		mat4 proj; // World View Projection
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
	void render(const std::vector<T>& vb, const std::vector<int>& ib, const ConstantBuffer& cb,
	            RenderTarget& render_target)
	{
		cb_ = cb;
		//		static const float max = DBL_MAX;
		for (auto&& value : zbuffer)
		{
			value = DBL_MAX;
		}
		IaStage(ib);
		VSStage(vb);
		//		EarlyZ();
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
			origin_pos.w = 1;
			vb_after_vs_[i].pos = origin_pos * cb_.WVP;
			//			vb_after_vs_[i].pos = vb_after_vs_[i].pos * cb_.proj;
			//			vb_after_vs_[i].pos = origin_pos * cb_.WVP;
			//				vb_after_vs_[i].depthPos = origin_pos * cb_.WVP;

			// ��һ��
			vb_after_vs_[i].pos.x = vb_after_vs_[i].pos.x / vb_after_vs_[i].pos.w;
			vb_after_vs_[i].pos.y = vb_after_vs_[i].pos.y / vb_after_vs_[i].pos.w;
			vb_after_vs_[i].pos.z = vb_after_vs_[i].pos.z / vb_after_vs_[i].pos.w;
//			vb_after_vs_[i].pos.w = vb_after_vs_[i].pos.w / vb_after_vs_[i].pos.w;
			//			vb_after_vs_[i].pos.data()[0] = vb_after_vs_[i].pos.data()[0] / vb_after_vs_[i].pos.data()[3];
			//			vb_after_vs_[i].pos.data()[1] = vb_after_vs_[i].pos.data()[1] / vb_after_vs_[i].pos.data()[3];
			//			vb_after_vs_[i].pos.data()[2] = vb_after_vs_[i].pos.data()[2] / vb_after_vs_[i].pos.data()[3];

			if constexpr (HAS_MEMBER(T, uv))
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
			static int shading(vertex_output& x)
			{
				return a;
				//				int a1 = int(abs(x.pos.x) * 200) & 0xFF;
				//				int a2 = int(abs(x.pos.y) * 200) & 0xFF;
				//				int a3 = int(abs(x.pos.z) * 200) & 0xFF;
				//				return ++a;
			}
		};
		for (int i = 0; i < triangles_.size(); ++i)
		{
			a = (0xFF * (i & 4) << 16) | (0xFF * (i & 2) << 8) | (0xFF * (i & 1));
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
			screen_cords[i].x = (vertices[i].pos.x + 1) / 2 * width_;
			screen_cords[i].y = (vertices[i].pos.y + 1) / 2 * height_;
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
			if (screen_cords[i].x < rect.left) rect.left = screen_cords[i].x;
			if (screen_cords[i].x > rect.right) rect.right = screen_cords[i].x;
			if (screen_cords[i].y < rect.top) rect.top = screen_cords[i].y;
			if (screen_cords[i].y > rect.bottom) rect.bottom = screen_cords[i].y;
		}

		rect.bottom = min(rect.bottom, height_);
		rect.top = max(rect.top, 0);
		rect.left = max(rect.left, 0);
		rect.right = min(rect.right, width_);

		//	static std::vector<uint8_t> in_line;
		//	in_line.resize(height_);
		//	std::vector<uint8_t> out_line;

		const auto& x1 = screen_cords[0].x;
		const auto& x2 = screen_cords[1].x;
		const auto& x3 = screen_cords[2].x;
		const auto& y1 = screen_cords[0].y;
		const auto& y2 = screen_cords[1].y;
		const auto& y3 = screen_cords[2].y;

		auto denominator = (y2 - y3) * (x1 - x3) + (x3 - x2) * (y1 - y3);
		if (denominator == 0)
			return;
		auto numerator1_1 = (y2 - y3) / denominator;
		auto numerator1_2 = (x3 - x2) / denominator;
		auto numerator2_1 = (y3 - x1) / denominator;
		auto numerator2_2 = (x1 - x3) / denominator;

		vec2 v0 = screen_cords[1] - screen_cords[0];
		vec2 v1 = screen_cords[2] - screen_cords[0];
		//		float d00 = v0.dot(v0);
		//		float d01 = v0.dot(v1);
		//		float d11 = v1.dot(v1);
		float denom = v0.x * v1.y - v1.x * v0.y;
		float inv_denom = 1.0f / denom;


		for (int y = rect.top; y < rect.bottom; ++y)
		{
			bool in_line = false;
			for (int x = rect.left; x < rect.right; ++x)
			{
				vec2 p(x, y);
				vec2 v2 = p - screen_cords[0];
				auto lambda2 = (v2.x * v1.y - v1.x * v2.y) * inv_denom;
				auto lambda3 = (v0.x * v2.y - v2.x * v0.y) * inv_denom;
				auto lambda1 = 1.0f - lambda2 - lambda3;


				//				auto lambda1 = numerator1_1 * (x - x3) + numerator1_2 * (y - y3);
				//				auto lambda2 = numerator2_1 * (x - x3) + numerator2_2 * (y - y3);
				//				auto lambda3 = 1 - lambda1 - lambda2;
				const bool is_in = 
					lambda1 < 1.0f - DBL_EPSILON && lambda1 > DBL_EPSILON &&
					lambda2 < 1.0f - DBL_EPSILON && lambda2 > DBL_EPSILON && 
					lambda3 < 1.0f - DBL_EPSILON && lambda3 > DBL_EPSILON;
				if (is_in)
				{
					// do
					vertex_output interpolated;
					interpolated.pos = vertices[0].pos * lambda1 + vertices[1].pos * lambda2 + vertices[2].pos * lambda3;


					// Z-PASS
					auto& z = interpolated.pos.z;
					if (z > 0 && z < 1 && z <= zbuffer[y * width_ + x])
					{
						zbuffer[y * width_ + x] = z;
						auto zr =
							lambda1 / vertices[0].pos.z +
							lambda2 / vertices[1].pos.z +
							lambda3 / vertices[2].pos.z;
						auto lambda1_c = lambda1 / vertices[0].pos.z / zr;
						auto lambda2_c = lambda2 / vertices[1].pos.z / zr;
						auto lambda3_c = lambda3 / vertices[2].pos.z / zr;
						interpolated.uv = vertices[0].uv * lambda1_c + vertices[1].uv * lambda2_c + vertices[2].uv * lambda3_c;
						if constexpr (!std::is_void<decltype(PixelShader::shading(interpolated))>::value)
						{
							auto ret = PixelShader::shading(interpolated);
							render_target.DrawPoint(x, y, ret);
						}
					}
//
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
