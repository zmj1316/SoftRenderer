#pragma once
#include "math3d.hpp"
#include "helpers.hpp"
#include <vector>

constexpr float EPSILON = 1e-5f;

template<typename ConstantBuffer, class VertexShader, class PixelShader>
class Renderer
{
public:
	using VertexOutput = typename VertexShader::VertexOutput;

	Renderer();
	~Renderer();

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
		Clear();
		IaStage(ib);
		VSStage(vb);
//		EarlyZ();
		PSStage(render_target);
	}

protected:

	struct polygon_t
	{
		int indices[3];
	};

	ConstantBuffer cb_;
	std::vector<polygon_t> triangles_;
	std::vector<VertexOutput> vb_after_vs_;
	std::vector<float> zbuffer;
	int width_;
	int height_;

	void Clear();
	void IaStage(const std::vector<int>& ib);


	template <typename T>
	void VSStage(const std::vector<T>& vb);

	void EarlyZ();

	template <class RenderTarget>
	void PSStage(RenderTarget& render_target);


	template <typename _PixelShader, class RenderTarget>
	void RasterizeTriangle(int index, RenderTarget& render_target);
};




template<typename ConstantBuffer, class VertexShader, class PixelShader>
Renderer<ConstantBuffer, VertexShader, PixelShader>::Renderer() : width_(0), height_(0)
{
}


template<typename ConstantBuffer, class VertexShader, class PixelShader>
Renderer<ConstantBuffer, VertexShader, PixelShader>::~Renderer()
{
}

template<typename ConstantBuffer, class VertexShader, class PixelShader>
void Renderer<ConstantBuffer, VertexShader, PixelShader>::Clear()
{
	for (auto&& value : zbuffer)
	{
		value = 1.0f;
	}
}

template<typename ConstantBuffer, class VertexShader, class PixelShader>
void Renderer<ConstantBuffer, VertexShader, PixelShader>::IaStage(const std::vector<int>& ib)
{
	MY_ASSERT((ib.size() % 3) == 0);
	triangles_.resize(ib.size() / 3);
	for (int i = 0; i < triangles_.size(); ++i)
	{
		for (int r = 0; r < 3; ++r)
		{
			triangles_[i].indices[r] = ib[i * 3 + r];
		}
	}
}

template <typename ConstantBuffer, class VertexShader, class PixelShader>
template <typename T>
void Renderer<ConstantBuffer, VertexShader, PixelShader>::VSStage(const std::vector<T>& vb)
{
	vb_after_vs_.resize(vb.size());

	for (int i = 0; i < vb.size(); ++i)
	{
		vb_after_vs_[i] = VertexShader::shading(vb[i], cb_);
		// ¹éÒ»»¯
		vb_after_vs_[i].pos.x = vb_after_vs_[i].pos.x / vb_after_vs_[i].pos.w;
		vb_after_vs_[i].pos.y = vb_after_vs_[i].pos.y / vb_after_vs_[i].pos.w;
		vb_after_vs_[i].pos.z = vb_after_vs_[i].pos.z / vb_after_vs_[i].pos.w;
	}
}

template<typename ConstantBuffer, class VertexShader, class PixelShader>
void Renderer<ConstantBuffer, VertexShader, PixelShader>::EarlyZ()
{
	class EmptyPASS
	{
	public:
		static void shading(VertexOutput& x, ConstantBuffer& cb)
		{
		}
	};

	for (int i = 0; i < triangles_.size(); ++i)
	{
		auto empry = EmptyPASS();
		RasterizeTriangle<EmptyPASS>(i, empry);
	}
}

template <typename ConstantBuffer, class VertexShader, class PixelShader>
template <class RenderTarget>
void Renderer<ConstantBuffer, VertexShader, PixelShader>::PSStage(RenderTarget& render_target)
{
	for (int i = 0; i < triangles_.size(); ++i)
	{
		RasterizeTriangle<PixelShader>(i, render_target);
	}
}

template <typename ConstantBuffer, class VertexShader, class PixelShader>
template <typename _PixelShader, class RenderTarget>
void Renderer<ConstantBuffer, VertexShader, PixelShader>::RasterizeTriangle(int index, RenderTarget& render_target)
{
	auto& tri = triangles_[index];
	VertexOutput vertices[3];
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
	float denom = v0.x * v1.y - v1.x * v0.y;
	float inv_denom = 1.0f / denom;

	int max_x = min(int(rect.right + 1), int(width_));
	int max_y = min(int(rect.bottom + 1), int(height_));

	int min_x = int(rect.left);
	int min_y = int(rect.top);

	float v1y_inv_denom = v1.y * inv_denom;
	float v1x_inv_denom = -v1.x * inv_denom;
	float v0x_inv_denom = v0.x * inv_denom;
	float v0y_inv_denom = -v0.y * inv_denom;

	for (int y = rect.top; y < max_y; ++y)
	{
		bool in_line = false;
		for (int x = rect.left; x < max_x; ++x)
		{
			vec2 p(x, y);
			vec2 v2 = p - screen_cords[0];

//			bool is_in = true;
			auto lambda2 = v2.x * v1y_inv_denom + v2.y * v1x_inv_denom;
//			auto lambda2 = (v2.x * v1.y - v1.x * v2.y) * inv_denom;
//			if( lambda2 > 1 || lambda2 < 0)
//			if (fast_judge(lambda2))
//			{
//				if (in_line)
//				{
//					break;// finish line
//				}
//				else
//				{
//					continue;
//				}
//			}
			auto lambda3 = v2.y * v0x_inv_denom + v2.x * v0y_inv_denom;
//			auto lambda3 = (v0.x * v2.y - v2.x * v0.y) * inv_denom;
//			if (lambda3 > 1 || lambda3 < 0)
//			if (fast_judge(lambda3))
//
//			{
//				if (in_line)
//				{
//					break;// finish line
//				}
//				else
//				{
//					continue;
//				}
//			}
			auto lambda1 = 1.0f - (lambda2 + lambda3);
//			if (lambda1 > 1 || lambda1 < 0)
			if (fast_judge(lambda1) || fast_judge(lambda2) || fast_judge(lambda3))
			{

				if (in_line)
				{
					break;// finish line
				}
				else
				{
					continue;
				}
			}

//			auto abss = fabs(lambda1) + fabs(lambda2) + fabs(lambda3);
//			const bool is_in =
//				fabs(1 - (fabs(lambda1) + fabs(lambda2) + fabs(lambda3))) <= EPSILON;
//			const bool is_in =
//				lambda1 > 0 && lambda1 < 1 &&
//				lambda2 > 0 && lambda2 < 1 &&
//				lambda3 > 0 && lambda3 < 1;
//			if (is_in != is_in2)
//				continue;
//			if (is_in)
			{
				// do
				VertexOutput interpolated;
				interpolated.pos = vertices[0].pos * lambda1 + vertices[1].pos * lambda2 + vertices[2].pos * lambda3;


				// Z-PASS
				auto& z = interpolated.pos.z;
				if (z > 0 && z <= zbuffer[y * width_ + x])
				{
					if (
						vertices[0].pos.w < 0 ||
						vertices[1].pos.w < 0 ||
						vertices[2].pos.w < 0
					)
						continue;
					in_line = true;
					zbuffer[y * width_ + x] = z;

					auto zr = 
						fabs(lambda1 / vertices[0].pos.w) +
						fabs(lambda2 / vertices[1].pos.w) +
						fabs(lambda3 / vertices[2].pos.w);
					zr = 1 / zr;
					auto lambda1_c = lambda1 / vertices[0].pos.w * zr;
					auto lambda2_c = lambda2 / vertices[1].pos.w * zr;
					auto lambda3_c = lambda3 / vertices[2].pos.w * zr;
					interpolated.uv = vertices[0].uv * lambda1_c + vertices[1].uv * lambda2_c + vertices[2].uv * lambda3_c;
					if constexpr (!std::is_void<decltype(_PixelShader::shading(interpolated, cb_))>::value)
					{
						auto ret = _PixelShader::shading(interpolated, cb_);
						render_target.DrawPoint(x, y, ret);
					}
				}
			}
//			else if (in_line)
//			{
//				break;// finish line
//			}
		}
	}
}
