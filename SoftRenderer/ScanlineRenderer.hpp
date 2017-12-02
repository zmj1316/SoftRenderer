#pragma once
#include "Renderer.hpp"
#include <vector>
#include <unordered_map>

template<typename ConstantBuffer, class VertexOutput, class PixelShader, bool LineMode>
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
		float y_top;
		bool flag;
	};

	struct edge_entry
	{
		float x_left;
		float z_left;
		float y_top;
		float dx;
		float dz;
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

	struct active_poly_entry {
		uint32_t id;
		float z;
	};

	std::vector<poly_entry> pt_list;
	std::vector<std::vector<poly_entry>> pt_;
	std::vector<std::vector<edge_entry>> et_;
	std::unordered_map<uint32_t,float> ipl_;
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

		pt_list.clear();
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
			pe.color = vertices[0].world_pos;
			pe.y_top = screen_cords[y_max_id].y;
			pt_list.push_back(pe);
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

				if(point0.y == y_buttom)
				{
					ee.z_left = vertices[edge_id].pos.z;
					ee.dz = (vertices[(edge_id + 1) % 3].pos.z - vertices[edge_id].pos.z) / (y_top - y_buttom);
				}
				else
				{
					ee.z_left = vertices[(edge_id + 1) % 3].pos.z;
					ee.dz = -(vertices[(edge_id + 1) % 3].pos.z - vertices[edge_id].pos.z) / (y_top - y_buttom);
				}

				et_[y_buttom].push_back(ee);
			}
		}
	}

	uint32_t doRGB(vec3 color){
		return ((uint32_t)color.x * 0xFF) << 16 | ((uint32_t)color.y*0xFF) << 8 | ((uint32_t)color.z*0xFF);
	}

	template <class RenderTarget>
	void PSStage(RenderTarget& render_target)
	{
		buildTable();
		ipl_.clear();
		for (int scan_y = 0; scan_y < height_; ++scan_y)
		{
			aet_.erase(std::remove_if(aet_.begin(), aet_.end(), [scan_y](const auto& edge)
			{
				return int(edge.y_top) <= scan_y;
			}), aet_.end());

			//ipl_.erase(std::remove_if(ipl_.begin(), ipl_.end(), [scan_y](const auto& pe)
			//{
			//	return int(pe.second.y_top) <= scan_y;
			//}), ipl_.end());

			for (auto && active_edge : aet_)
			{
				active_edge.x_left += active_edge.dx;
				active_edge.z_left += active_edge.dz;
			}

			for (auto && ee : et_[scan_y])
			{
				aet_.push_back(ee);
			}

			//for (auto && p : pt_[scan_y])
			//{
			//	ipl_.push_back(p);
			//}

			if(aet_.size() > 0)
			{
				struct segment
				{
					float l, r;
					float zl, zr;
				};

				std::unordered_map<uint32_t, segment> segments;
				for (auto & active_edge : aet_)
				{
					if(segments.find(active_edge.poly_id) == segments.end())
					{
						segment seg;
						seg.l = active_edge.x_left;
						seg.zl = active_edge.z_left;
						segments.insert_or_assign(active_edge.poly_id, seg);
					}
					else
					{
						auto& seg = segments[active_edge.poly_id];
						seg.r = active_edge.x_left;
						seg.zr = active_edge.z_left;
						if(seg.l > seg.r)
						{
							std::swap(seg.l, seg.r);
							std::swap(seg.zl, seg.zr);
						}
					}
				}

	#if _MSC_VER >= 1910

				if constexpr(LineMode){
					auto prev_left = 0.f;
					std::sort(aet_.begin(), aet_.end(), [](const auto& x, const auto & y) {return x.x_left < y.x_left; });
					for (auto && active_edge : aet_)
					{
						if(active_edge.x_left > prev_left && active_edge.x_left < width_)
						{

							int left = std::floorf(active_edge.x_left);
							int draw_id = -1;
							float z = 1;
							for (const auto& pair : ipl_) {
								if(pair.second < z) {
									z = pair.second;
									draw_id = pair.first;
								}
							}
							if (draw_id >= 0) {
								
							//if (!std::isfinite(active_edge.dx))
							//	continue;
//							if (left > right)
//								std::swap(left, right);
//							if (left < 0 || right > width_ - 1)
//								continue;
								for (int i = prev_left; i <= left; ++i)
								{
									render_target.DrawPoint(i, scan_y, int(0xFF * z));
								}
							}

							prev_left = left;
							int pid = active_edge.poly_id;
							if (ipl_.find(pid) != ipl_.end()) {
								ipl_.erase(pid);
							}
							else {
								ipl_.insert_or_assign(pid, active_edge.z_left);
							}
						}
					}
				}
	#endif // _MSC_VER 1910
			}
		}
	}
	
};

