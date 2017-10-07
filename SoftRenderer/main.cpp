#include <algorithm>
#include <windows.h>
#include "helpers.hpp"
#include "MYUT.hpp"
#include "RendererDevice.hpp"
#include "Renderer.hpp"

GDIDevice device;
Renderer renderer;

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


std::vector<vertex_> vb;
std::vector<int> ib;
Renderer::ConstantBuffer cb;

class Camera
{
public:
	vec3 up{0,1,0};
	vec3 at{0,0,0};
	vec3 eye{0,1,5};
};

static vec3 up{0,1,0};
static vec3 at{0,0,0};
static vec3 eye{3,0,5};
float height, width;

void init()
{
	//	vb.push_back({{0,0,0,1}});
	//	vb.push_back({{1,0,0,1}});
	//	vb.push_back({{0,1,0,1}});
	//	vb.push_back({{1,1,0,1}});
		vb =
		{
			{-1.0f, 1.0f, -1.0f, 1},
			{1.0f, 1.0f, -1.0f,1},
			{1.0f, 1.0f, 1.0f ,1},
			{-1.0f, 1.0f, 1.0f, 1},
			{-1.0f, -1.0f, -1.0f, 1},
			{1.0f, -1.0f, -1.0f, 1},
			{1.0f, -1.0f, 1.0f, 1},
			{-1.0f, -1.0f, 1.0f, 1},
			{-1.0f, -1.0f, 1.0f, 1},
			{-1.0f, -1.0f, -1.0f, 1},
			{-1.0f, 1.0f, -1.0f, 1},
			{-1.0f, 1.0f, 1.0f, 1},
	
			{1.0f, -1.0f, 1.0f, 1},
			{1.0f, -1.0f, -1.0f, 1},
			{1.0f, 1.0f, -1.0f, 1},
			{1.0f, 1.0f, 1.0f, 1},
	
			{-1.0f, -1.0f, -1.0f, 1},
			{1.0f, -1.0f, -1.0f, 1},
			{1.0f, 1.0f, -1.0f, 1},
			{-1.0f, 1.0f, -1.0f, 1},
	
			{-1.0f, -1.0f, 1.0f, 1},
			{1.0f, -1.0f, 1.0f, 1},
			{1.0f, 1.0f, 1.0f, 1},
			{-1.0f, 1.0f, 1.0f, 1},
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

//	vb =
//	{
//		{1,1,1,1},
//		{1,-1,1,1},
//		{1,-1,-1,1},
//		{1,1,-1,1},
//
//		{-1,1,1,1},
//		{-1,-1,1,1},
//		{-1,-1,-1,1},
//		{-1,1,-1,1},
//	};
//
//	ib = 
//	{
//		0,1,2,
//		2,3,0,
//		4,5,6,
//		6,7,4,
//
//	};
}

void calcCamera()
{
	//	//	vec4 z_axis = XMVector3Normalize(at - eye);
	//	vec3 z_axis = (at - eye).normalized();
	//	vec3 x_axis = cross(up, z_axis).normalized();
	//	//	auto x_axis = XMVector3Normalize(XMVector3Cross(up, z_axis));
	//	//	auto y_axis = XMVector3Normalize(XMVector3Cross(z_axis, x_axis));
	//	vec3 y_axis = cross(z_axis, x_axis).normalized();
	//	auto x = (float*)&x_axis;
	//	auto y = (float*)&y_axis;
	//	auto z = (float*)&z_axis;
	//	float view[]{
	//		x[0],y[0],z[0],0,
	//		x[1],y[1],z[1],0,
	//		x[2],y[2],z[2],0,
	//		-x_axis.dot(eye),-y_axis.dot(eye),-z_axis.dot(eye),1 };
	//	float Near = 0.5f;
	//	float Far = 1000.0f;
	//	float cot_f = 3;
	//	float aspect = 4 / 3;
	//	float proj[]{
	//		cot_f * height / width,0,0,0,
	//		0,cot_f,0,0,
	//		0,0,Far / (Far - Near),1,
	//		0,0,Far*Near / (Near - Far),0
	//	};
	mat4 viewm, projm;
	viewm = mat4::lookAtLH(eye, at, up);
	projm = mat4::perspectiveFovLH(3.14 / 3, width / height, 0.5, 100);
	//	memcpy(&viewm, view, sizeof(view));
	//	memcpy(&projm, proj, sizeof(proj));
	cb.WVP = viewm * projm;
	cb.view = viewm;
	cb.proj = projm;
}

LRESULT CALLBACK draw()
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
	calcCamera();
	renderer.render(vb, ib, cb, device);
	device.RenderToScreen();
	return S_OK;
}

LRESULT CALLBACK resize(int w, int h)
{
	renderer.resizeRenderTarget(w, h);
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
