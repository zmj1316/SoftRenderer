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
		float x_top;
		float y_top;
		float dx;
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

	PolyTable pt_;
	EdgeTable et_;
	InPolyList ipl_;
	ActiveEdgeTable aet_;

	void Clear()
	{
		pt_.clear();
		et_.clear();
		pt_.resize(height_);
		et_.resize(height_);
	}
	
	void buildTable()
	{
		for (const auto & tri : triangles_)
		{
			
		}
	}


	template <class RenderTarget>
	void PSStage(RenderTarget& render_target)
	{
		
	}
	
};

