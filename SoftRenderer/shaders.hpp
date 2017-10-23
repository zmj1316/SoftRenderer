#pragma once
#include "math3d.hpp"
#include "helpers.hpp"
#include "sampler.hpp"


struct ConstantBuffer
{
	mat4 WVP;
	vec3 light_pos;
	vec3 view_pos;
};

class VertexShader
{
public:
	struct VertexOutput
	{
		vec4 pos;
		vec4 worldpos;
		vec3 normal;
		vec2 uv;
	};

	DEF_HAS_MEMBER(uv)


	template <typename Vertex, class ConstantBuffer>
	static VertexOutput shading(const Vertex& input, const ConstantBuffer& cb)
	{
		VertexOutput out;
		vec4 origin_pos = input.pos;
		origin_pos.w = 1;
		out.pos = origin_pos * cb.WVP;
		out.worldpos = origin_pos;

		if constexpr (HAS_MEMBER(Vertex, uv))
		{
			out.uv = input.uv;
		}
		return out;
	}
};

class PixelShader
{
public:

	static int doRGB(vec3 v)
	{
		v = clamp(v,vec3(0,0,0), vec3(255, 255, 255));
		return
			int(v.x) << 16 |
			int(v.y) << 8 |
			int(v.z);

	}

	static int shading(VertexShader::VertexOutput& x, ConstantBuffer& cb)
	{
//		return (0xFF & int(pow(x.pos.z, 8) * 255)) << 16;
		static sampler s0("tex.bmp");
//		return (int(x.uv.x * 255)<<8) | int(x.uv.y * 255) ;
//		return doRGB(s0.sample(x.uv.x, x.uv.y));
		const auto color = s0.sample(x.uv.x, x.uv.y);
		const auto world_pos = x.worldpos.xyz();
		auto light_dir = world_pos - cb.light_pos;
		auto view_dir = world_pos - cb.view_pos;

		auto diff = light_dir.normalized().dot(x.normal);
		auto reflect = (x.normal)* diff * 2.0f -light_dir.normalized();

		float tmp2 = reflect.normalized().dot(view_dir.normalized());
		auto spec = pow(tmp2, 16);

		return doRGB(color * diff + vec3(255,255,255) * spec);
	}

};
