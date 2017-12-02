#include <algorithm>
#include <windows.h>
#include "helpers.hpp"
#include "MYUT.hpp"
#include "RendererDevice.hpp"
#include "Renderer.hpp"
#include "ScanlineRenderer.hpp"
#include "mmdformat/Pmx.h"

GDIDevice device;

#pragma pack(16)
struct vertex_
{
	vec4 pos;

	vertex_(float x, float y, float z, float w): pos(x, y, z, w)
	{
	}

	vertex_()
	{
	};
};

struct vertex_output
{
	vec4 pos;
	vec2 uv;
	vec3 world_pos;
};

struct ConstantBuffer
{
	mat4 WVP;
};

class PixelShader
{
public:
	static int shading(vertex_output& x, ConstantBuffer)
	{
		return (0xFF & int(pow(x.pos.z, 8) * 255)) << 16;
	}
};

std::vector<vertex_> vb;
std::vector<int> ib;
ConstantBuffer cb;
bool scan = false;

static vec3 up{0,1,0};
static vec3 at{0,1,0};
static vec3 eye{3,0,5};
float height, width;

Renderer<ConstantBuffer, vertex_output, PixelShader> renderer;
ScanlineRenderer<ConstantBuffer, vertex_output, PixelShader, true> scan_renderer;

void init()
{
	pmx::PmxModel model_;
	std::wstring filename = L"TDA.pmx";
	std::ifstream stream = std::ifstream(filename, std::ios_base::binary);
	model_.Read(&stream);
	auto vs = model_.vertices.get();
	for (int i = 0; i < model_.vertex_count; ++i)
	{
		auto it = vs + i;
		//std::unique_ptr<pmx::PmxVertexSkinning> p2(std::move(it->skinning));
		auto sk = (pmx::PmxVertexSkinningBDEF1*) it->skinning.get();
		auto sk2 = (pmx::PmxVertexSkinningBDEF2*) it->skinning.get();
		auto sk4 = (pmx::PmxVertexSkinningBDEF4*) it->skinning.get();
		auto sks = (pmx::PmxVertexSkinningSDEF*) it->skinning.get();
		vertex_ v;
		v.pos.x = it->positon[0];
		v.pos.y = -it->positon[1];
		v.pos.z = it->positon[2];
		vb.push_back(v);
	}
	for (int i = 0; i < model_.index_count; ++i)
	{
		ib.push_back(model_.indices.get()[i]);
	}
	//vb =
	//{
	//	{-1.0f, 1.0f, -1.0f, 1},
	//	{1.0f, 1.0f, -1.0f,1},
	//	{1.0f, 1.0f, 1.0f ,1},
	//	{-1.0f, 1.0f, 1.0f, 1},
	//	{-1.0f, -1.0f, -1.0f, 1},
	//	{1.0f, -1.0f, -1.0f, 1},
	//	{1.0f, -1.0f, 1.0f, 1},
	//	{-1.0f, -1.0f, 1.0f, 1},
	//	{-1.0f, -1.0f, 1.0f, 1},
	//	{-1.0f, -1.0f, -1.0f, 1},
	//	{-1.0f, 1.0f, -1.0f, 1},
	//	{-1.0f, 1.0f, 1.0f, 1},

	//	{1.0f, -1.0f, 1.0f, 1},
	//	{1.0f, -1.0f, -1.0f, 1},
	//	{1.0f, 1.0f, -1.0f, 1},
	//	{1.0f, 1.0f, 1.0f, 1},

	//	{-1.0f, -1.0f, -1.0f, 1},
	//	{1.0f, -1.0f, -1.0f, 1},
	//	{1.0f, 1.0f, -1.0f, 1},
	//	{-1.0f, 1.0f, -1.0f, 1},

	//	{-1.0f, -1.0f, 1.0f, 1},
	//	{1.0f, -1.0f, 1.0f, 1},
	//	{1.0f, 1.0f, 1.0f, 1},
	//	{-1.0f, 1.0f, 1.0f, 1},
	//};
	//ib =
	//{
	//	3,1,0,
	//	2,1,3,

	//	6,4,5,
	//	7,4,6,

	//	11,9,8,
	//	10,9,11,

	//	14,12,13,
	//	15,12,14,

	//	19,17,16,
	//	18,17,19,

	//	22,20,21,
	//	23,20,22
	//};
}

void calcCamera()
{
	mat4 viewm, projm;
	viewm = mat4::lookAtLH(eye, at, up);
	projm = mat4::perspectiveFovLH(3.14 / 3, width / height, 0.5, 100);
	cb.WVP = viewm * projm;
}

void handleIO()
{
	if (MYUTGetKeys()[VK_UP])
		at.y += 0.1;
	if (MYUTGetKeys()[VK_DOWN])
		at.y -= 0.1;
	if (MYUTGetKeys()[VK_LEFT])
		at.x += 0.1;
	if (MYUTGetKeys()[VK_RIGHT])
		at.x -= 0.1;

	if (MYUTGetKeys()['W'])
		eye.y += 0.1;
	if (MYUTGetKeys()['S'])
		eye.y -= 0.1;
	if (MYUTGetKeys()['A'])
		eye.x += 0.1;
	if (MYUTGetKeys()['D'])
		eye.x -= 0.1;

	scan = true;
	if (MYUTGetKeys()['Z'])
		scan = false;

	if (MYUTGetKeys()['Q'])
	{
		auto dir = (eye - at).normalized();
		eye += dir * 0.1;
		at += dir * 0.1;
	}
	if (MYUTGetKeys()['E'])
	{
		auto dir = (eye - at).normalized();
		eye -= dir * 0.1;
		at -= dir * 0.1;
	}
}

LRESULT CALLBACK draw()
{
	handleIO();
	calcCamera();
	if(scan)
		scan_renderer.render(vb, ib, cb, device);
	else
		renderer.render(vb, ib, cb, device);
	device.RenderToScreen();
	return S_OK;
}

LRESULT CALLBACK resize(int w, int h)
{
	renderer.resizeRenderTarget(w, h);
	scan_renderer.resizeRenderTarget(w, h);
	device.Resize(w, h);
	height = h;
	width = w;
	calcCamera();
	return S_OK;
}


int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	// Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
	MYUTCreateWindow(L"test2");
	MYUTSetCallBackDraw(&draw);
	MYUTSetCallBackResize(&resize);
	init();
	resize(640, 480);
	MYUTMainLoop();
	return 0;
}
