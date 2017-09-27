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
		mat4 WVP;  // World View Projection
		vec3 lightPos;
		vec3 lightColor;
	};

	void resizeRenderTarget(int width, int height)
	{
		width_ = width;
		height_ = height;
		zbuffer.resize(width * height);
	}

	template<typename T>
	void render(const std::vector<T>& vb,const std::vector<int>& ib, const ConstantBuffer& cb)
	{
		cb_ = cb;
		IaStage(ib);
		VSStage(vb);
		EarlyZ();
	}

protected:
	struct polygon_
	{
		int indices[3];
	};

	struct vertex_output
	{
		vec4 pos;
		vec4 depthPos;
	};

	void Clear();
	void IaStage(const std::vector<int>& ib);

	DEF_HAS_MEMBER(pos)
	template<typename T>
	void VSStage(const std::vector<T>& vb)
	{
		vb_after_vs_.resize(vb.size());
		if constexpr(HAS_MEMBER(T, pos) && std::is_same<decltype(T::pos),vec3>::value)
		{
			for (int i = 0; i < vb.size(); ++i)
			{
				const vec4 origin_pos = vb[i].pos;
				origin_pos.w = 1;
				vb_after_vs_[i].pos = origin_pos * cb_.WVP;
				vb_after_vs_[i].depthPos = origin_pos * cb_.WVP;

				// ¹éÒ»»¯
				vb_after_vs_[i].pos.x = vb_after_vs_[i].pos.x / vb_after_vs_[i].pos.w;
				vb_after_vs_[i].pos.y = vb_after_vs_[i].pos.y / vb_after_vs_[i].pos.w;
				vb_after_vs_[i].pos.z = vb_after_vs_[i].pos.z / vb_after_vs_[i].pos.w;
			}
		}
	}

	void EarlyZ();
	void PSStage();

	void RasterizeTriangle(int index);

	ConstantBuffer cb_;
	std::vector<polygon_> triangles_;
	std::vector<vertex_output> vb_after_vs_;
	std::vector<float> zbuffer;
	int width_;
	int height_;



};

