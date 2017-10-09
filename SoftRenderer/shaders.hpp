#pragma once
#include "math3d.hpp"
#include "helpers.hpp"

struct ConstantBuffer
{
	mat4 WVP;
};

class VertexShader
{
public:
	struct VertexOutput
	{
		vec4 pos;
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
	static int shading(VertexShader::VertexOutput& x, ConstantBuffer)
	{
		return (0xFF & int(pow(x.pos.z, 8) * 255)) << 16;
	}
};
