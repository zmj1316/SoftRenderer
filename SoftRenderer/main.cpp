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
	vec3 normal;

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
	vec3 normal;
};

struct ConstantBuffer
{
	mat4 WVP;
	vec3 light_dir;
};

class PixelShader
{
public:

	static uint32_t doRGB(vec3 color) {
		color = clamp(color, { 0,0,0 }, { 1,1,1 });
		return ((uint32_t)(color.x * 0xFF)) << 16 | ((uint32_t)(color.y * 0xFF)) << 8 | ((uint32_t)(color.z * 0xFF));
	}

	static uint32_t shading(vertex_output& x, ConstantBuffer cb)
	{
		//return (0xFF & int(pow(x.pos.z * x.pos.z, 8) * 255)) << 16;
		return doRGB(vec3(x.normal.dot(cb.light_dir)));
	}
};

std::vector<vertex_> vb;
std::vector<int> ib;
ConstantBuffer cb;
bool scan = false;

static vec3 up{0,1,0};
static vec3 at{0,-5,0};
static vec3 eye{4,-10,20};
static float distance = 5.0f;
float height, width;

Renderer<ConstantBuffer, vertex_output, PixelShader> renderer;
ScanlineRenderer<ConstantBuffer, vertex_output, PixelShader, true> scan_renderer;
LARGE_INTEGER t0, t1, tf;

void init()
{
	QueryPerformanceFrequency(&tf);
	QueryPerformanceCounter(&t0);
	QueryPerformanceCounter(&t1);
	pmx::PmxModel model_;
	std::wstring filename = L"TDA.pmx";
	std::ifstream stream = std::ifstream(filename, std::ios_base::binary);
	if(!stream.is_open())
	{
		MessageBox(NULL, L"需要载入模型文件TDA.pmx", 0, 0);
		vb =
		{
			{ -1.0f, 1.0f, -1.0f, 1 },
			{ 1.0f, 1.0f, -1.0f,1 },
			{ 1.0f, 1.0f, 1.0f ,1 },
			{ -1.0f, 1.0f, 1.0f, 1 },
			{ -1.0f, -1.0f, -1.0f, 1 },
			{ 1.0f, -1.0f, -1.0f, 1 },
			{ 1.0f, -1.0f, 1.0f, 1 },
			{ -1.0f, -1.0f, 1.0f, 1 },
			{ -1.0f, -1.0f, 1.0f, 1 },
			{ -1.0f, -1.0f, -1.0f, 1 },
			{ -1.0f, 1.0f, -1.0f, 1 },
			{ -1.0f, 1.0f, 1.0f, 1 },

			{ 1.0f, -1.0f, 1.0f, 1 },
			{ 1.0f, -1.0f, -1.0f, 1 },
			{ 1.0f, 1.0f, -1.0f, 1 },
			{ 1.0f, 1.0f, 1.0f, 1 },

			{ -1.0f, -1.0f, -1.0f, 1 },
			{ 1.0f, -1.0f, -1.0f, 1 },
			{ 1.0f, 1.0f, -1.0f, 1 },
			{ -1.0f, 1.0f, -1.0f, 1 },

			{ -1.0f, -1.0f, 1.0f, 1 },
			{ 1.0f, -1.0f, 1.0f, 1 },
			{ 1.0f, 1.0f, 1.0f, 1 },
			{ -1.0f, 1.0f, 1.0f, 1 },
		};
		ib =
		{
			3,1,0,
			2,1,3,

			6,4,5,
			7,4,6,

			11,9,8,
			10,9,11,

			14,12,13,
			15,12,14,

			19,17,16,
			18,17,19,

			22,20,21,
			23,20,22
		};
		return;
	}
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
		v.pos.z = -it->positon[2];

		v.normal.x = it->normal[0];
		v.normal.y = -it->normal[1];
		v.normal.z = -it->normal[2];
		vb.push_back(v);
	}
	for (int i = 0; i < model_.index_count; ++i)
	{
		ib.push_back(model_.indices.get()[i]);
	}
	return;
}

void calcCamera()
{
	mat4 viewm, projm;
	viewm = mat4::lookAtLH(eye, at, up);
	projm = mat4::perspectiveFovLH(3.14 / 3, width / height, 0.5, 1000);
	cb.WVP = viewm * projm;
	cb.light_dir = vec3(1, 1, 5).normalized();
}

void handleIO()
{

	QueryPerformanceFrequency(&tf);
	QueryPerformanceCounter(&t1);
	auto delta = 0.01 * (t1.QuadPart - t0.QuadPart) / tf.QuadPart;
	if (MYUTGetKeys()[VK_UP])
		at.y -= delta;
	if (MYUTGetKeys()[VK_DOWN])
		at.y += delta;
	if (MYUTGetKeys()[VK_LEFT])
		at.x += delta;
	if (MYUTGetKeys()[VK_RIGHT])
		at.x -= delta;

	if (MYUTGetKeys()['W'])
		eye.y -= delta;
	if (MYUTGetKeys()['S'])
		eye.y += delta;
	if (MYUTGetKeys()['A'])
		eye.x += delta;
	if (MYUTGetKeys()['D'])
		eye.x -= delta;

	scan = true;
	if (MYUTGetKeys()['Z'])
		scan = false;

	if (MYUTGetKeys()['Q'])
	{
		auto dir = (eye - at).normalized();
		eye += dir * delta;
		at += dir * delta;

		distance += delta;
	}
	if (MYUTGetKeys()['E'])
	{
		auto dir = (eye - at).normalized();
		eye -= dir * delta;
		at -= dir * delta;

		distance -= delta;
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
