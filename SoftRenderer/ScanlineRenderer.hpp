#pragma once
#include "Renderer.hpp"
#include <vector>
#include <map>

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
		//bool flag = false;
		float z;
		vec3 normal;
	};

	struct edge_entry
	{
		float x_left;
		int y_top;
		float z_left;
		float dx;
		float dz;
		uint32_t poly_id;
	};

	//struct active_edge_entry
	//{
	//	int xl;
	//	float dxl, dyl;
	//};

	using PolyTable = std::vector<std::vector<poly_entry>>;
	using EdgeTable = std::vector<std::vector<edge_entry>>;
	using InPolyList = std::vector<poly_entry>;
	using ActiveEdgeTable = std::vector<edge_entry>;

	//struct active_poly_entry {
	//	uint32_t id;
	//	float z;
	//};

	std::vector<poly_entry> pt_list;
	std::vector<std::vector<poly_entry>> pt_;
	std::vector<std::vector<edge_entry>> et_;
	std::multimap<float, uint32_t> ipl_;
	std::vector<edge_entry> aet_;


	//class IPLSpeedUp
	//{
	//private:
	//	std::map<uint32_t, float> index_to_z;


	//public:
	//	void Clear()
	//	{
	//		
	//	}

	//	void Update(uint32_t poly_id, float z)
	//	{
	//		
	//	}

	//	void Insert(uint32_t poly_id, float z)
	//	{
	//		index_to_z.insert_or_assign(poly_id, z);
	//	}

	//	void Remove(uint32_t poly_id)
	//	{
	//		index_to_z.erase(poly_id);
	//	}

	//	float nearest_z;
	//	uint32_t nearest_poly_id;
	//};

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

		for (size_t poly_id = 0; poly_id < triangles_.size(); ++poly_id)
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
			//pe.id = poly_id;
//			pe.flag = false;
//			pe.color = vertices[0].world_pos;
			pe.y_top = screen_cords[y_max_id].y;
			pe.z = (vertices[0].pos.z + vertices[1].pos.z + vertices[2].pos.z) / 3;
			//int y_min = std::floorf(screen_cords[y_min_id].y);
			//int y_max = std::floorf(screen_cords[y_max_id].y);
			//if (y_min < 0 || y_min >= height_)
			//	continue;
			//y_min = clamp(y_min, 0, height_ - 1);
			pe.normal = ((vertices[0].normal + vertices[1].normal + vertices[2].normal) / 3).normalized();
			pt_list.push_back(pe);
			auto normal = cross(vertices[0].pos.xyz() - vertices[1].pos.xyz(), vertices[0].pos.xyz() - vertices[2].pos.xyz());

			if (vertices[0].pos.w <= 0 || vertices[1].pos.w <= 0 || vertices[2].pos.w <= 0 || normal.z > 0)
			{
				pe.z = 100;
				//pt_[y_min].push_back(pe);
				continue;
			}
			//pt_[y_min].push_back(pe);

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
					//ee.z_left = vertices[edge_id].pos.z;
					ee.dz = (vertices[(edge_id + 1) % 3].pos.z - vertices[edge_id].pos.z) / (y_top - y_buttom);
				}
				else
				{
					//ee.z_left = vertices[(edge_id + 1) % 3].pos.z;
					ee.dz = -(vertices[(edge_id + 1) % 3].pos.z - vertices[edge_id].pos.z) / (y_top - y_buttom);
				}
				ee.z_left = pe.z;
				et_[y_buttom].push_back(ee);

			}
		}
	}

	uint32_t doRGB(vec3 color){
		color = clamp(color, { 0,0,0 }, { 1,1,1 });
		return ((uint32_t)(color.x * 0xFF)) << 16 | ((uint32_t)(color.y*0xFF)) << 8 | ((uint32_t)(color.z*0xFF));
	}

	template <class RenderTarget>
	void PSStage(RenderTarget& render_target)
	{
		buildTable();

		for (int scan_y = 0; scan_y < height_; ++scan_y)
		{
			for (auto && active_edge : aet_)
			{
				active_edge.x_left += active_edge.dx;
			}
			for (auto && ee : et_[scan_y])
			{
				aet_.push_back(ee);
			}
			aet_.erase(std::remove_if(aet_.begin(), aet_.end(), [scan_y](const auto& edge)
			{
				return int(edge.y_top) <= scan_y;
			}), aet_.end());


			if(aet_.size() > 0)
			{
#if _MSC_VER >= 1910
				if constexpr(LineMode)
#endif // _MSC_VER 1910
				{
					// sort the active edges by left_x
					std::sort(aet_.begin(), aet_.end(), [](const auto& x, const auto & y) {return x.x_left < y.x_left; });
					auto prev_left = (std::max)(int(aet_[0].x_left),0);

					for (auto && active_edge : aet_)
					{
						if(active_edge.x_left >= prev_left && prev_left < width_)
						{
							int left = std::floorf(active_edge.x_left);
							if (ipl_.size() > 0 && ipl_.begin()->first < 1) {
								auto z = ipl_.begin()->first;
								auto color = 0x303030 * ipl_.begin()->second;
								float diffuse = pt_list[ipl_.begin()->second].normal.dot(cb_.light_dir);
								for (int i = prev_left; i <= (std::min)(left,width_ - 1); ++i)
								{
									render_target.DrawPoint(i, scan_y, doRGB(vec3(diffuse)));
								}
							}

							prev_left = left;
						}
						int pid = active_edge.poly_id;
						auto it = std::find_if(ipl_.begin(), ipl_.end(), [pid](const auto& pair) {return pair.second == pid; });
						if(it == ipl_.end())
						{
							ipl_.insert(std::make_pair(active_edge.z_left, pid));
						}
						else
						{
							ipl_.erase(it);
						}
					}
				}
			}
		}
	}
	
};

