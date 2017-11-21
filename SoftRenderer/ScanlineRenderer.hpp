#pragma once
#include "Renderer.hpp"
#include <vector>

template<typename ConstantBuffer, class VertexOutput, class PixelShader>
class ScanlineRenderer: public Renderer<ConstantBuffer, VertexOutput, PixelShader>
{
public:



	template <typename T, class RenderTarget>
	void render(const std::vector<T>& vb, const std::vector<int>& ib, const ConstantBuffer& cb,
		RenderTarget& render_target)
	{
		cb_ = cb;
		Clear();
		IaStage(ib);
		VSStage(vb);
		PSStage(render_target);
	}
	void resizeRenderTarget(int width, int height)
	{
		width_ = width;
		height_ = height;
		zbuffer.resize(0);
		Clear();
	}

private:
	struct poly_entry
	{
		vec4 plane;
		uint32_t id;
		vec3 color;
		bool flag;
	};

	struct edge_entry
	{
		float x_left;
		float y_top;
		float dx;
		uint32_t poly_id;
	};

	struct active_edge_entry
	{
		int xl;
		float dxl, dyl;

	};

	using PolyTable = std::vector<std::vector<poly_entry>>;
	using EdgeTable = std::vector<std::vector<edge_entry>>;
	using InPolyList = std::vector<poly_entry>;
	using ActiveEdgeTable = std::vector<edge_entry>;

	std::vector<std::vector<poly_entry>> pt_;
	std::vector<std::vector<edge_entry>> et_;
	std::vector<poly_entry> ipl_;
	std::vector<edge_entry> aet_;

	void Clear()
	{
		pt_.clear();
		et_.clear();
		pt_.resize(height_);
		et_.resize(height_);
	}
	
	void buildTable()
	{
		pt_.clear();
		et_.clear();
		pt_.resize(height_);
		et_.resize(height_);
		ipl_.clear();
		aet_.clear();
		for (int poly_id = 0; poly_id < triangles_.size(); ++poly_id)
		{
			auto& tri = triangles_[poly_id];
			VertexOutput vertices[3];
			vec2 screen_cords[3];
			for (int i = 0; i < 3; ++i)
			{
				vertices[i] = vb_after_vs_[tri.indices[i]];
				screen_cords[i].x = (vertices[i].pos.x + 1) / 2 * width_;
				screen_cords[i].y = (vertices[i].pos.y + 1) / 2 * height_;
			}
			if (vertices[0].pos.w <= 0 || vertices[1].pos.w <= 0 || vertices[2].pos.w <= 0)
				continue;
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

			int y_max_id = 0;
			for (int i = 1; i < 3; ++i)
			{
				if (screen_cords[i].y > screen_cords[y_max_id].y)
					y_max_id = i;
			}

			int y_min_id = 0;
			for (int i = 1; i < 3; ++i)
			{
				if (screen_cords[i].y < screen_cords[y_max_id].y)
					y_min_id = i;
			}

			poly_entry pe;
			pe.id = poly_id;
			pe.flag = false;
			pe.color = vec3(1,1,1);

			int y_min = std::floorf(screen_cords[y_min_id].y);
			int y_max = std::floorf(screen_cords[y_max_id].y);
			//if (y_min < 0 || y_min >= height_)
			//	continue;
			y_min = clamp(y_min, 0, height_ - 1);
			pt_[y_min].push_back(pe);

			for (int edge_id = 0; edge_id < 3; ++edge_id)
			{
				auto& point0 = screen_cords[edge_id];
				auto& point1 = screen_cords[(edge_id + 1) % 3];
				edge_entry ee;
				float x_left = (std::min)(point0.x, point1.x);
				float x_right = (std::max)(point0.x, point1.x);
				float y_top = (std::max)(point0.y, point1.y);
				float y_buttom = (std::min)(point0.y, point1.y);
				if (int(y_buttom) >= height_ || int(y_top) < 0)
					continue;

				ee.poly_id = poly_id;
				ee.x_left = x_left;
				ee.y_top = y_top;


				if((  point0.x == x_left && point0.y == y_buttom)
					|| 
					  point0.x == x_right && point0.y == y_top)
					ee.dx = (x_right - x_left) / (y_top - y_buttom);
				else
				{
					ee.dx = -(x_right - x_left) / (y_top - y_buttom);
					ee.x_left = x_right;
				}

				if (int(y_buttom) < 0)
				{
					ee.x_left += -int(y_buttom) * ee.dx;
					y_buttom = 0;
				}
				et_[y_buttom].push_back(ee);
			}
		}
	}


	template <class RenderTarget>
	void PSStage(RenderTarget& render_target)
	{
		buildTable();
		for (int scan_y = 0; scan_y < height_; ++scan_y)
		{
			aet_.erase(std::remove_if(aet_.begin(), aet_.end(), [scan_y](const auto& edge)
			{
				return int(edge.y_top) <= scan_y;
			}), aet_.end());

			for (auto && active_edge : aet_)
			{
				active_edge.x_left += active_edge.dx;
			}

			for (auto && ee : et_[scan_y])
			{
				aet_.push_back(ee);
			}

			for (auto && active_edge : aet_)
			{
				if(active_edge.x_left > 0 && active_edge.x_left < width_)
					render_target.DrawPoint(active_edge.x_left, scan_y, 0xF * active_edge.poly_id | 0xFFF0FF00);
			}
		}
	}
	
};

