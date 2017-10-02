#include <algorithm>
#include <windows.h>
#include "helpers.hpp"
#include "MYUT.hpp"
#include "RendererDevice.hpp"
#include "Renderer.h"

GDIDevice device;
Renderer renderer;

struct vertex_
{
	vec4 pos;
};
std::vector<vertex_> vb;
std::vector<int> ib;
Renderer::ConstantBuffer cb;


LRESULT CALLBACK draw()
{

	renderer.render(vb, ib, cb, device);
	device.RenderToScreen();
	return S_OK;
}

LRESULT CALLBACK resize(int w, int h)
{
	renderer.resizeRenderTarget(w, h);
	device.Resize(w, h);
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
	device.Resize(640, 480);
	MYUTMainLoop();
	return 0;
}
