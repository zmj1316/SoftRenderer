// wrap native os api to simple draw pixel api
// now only win32 supported
// author:   Key Zhang

#pragma once
#include <vector>


class RendererDevice
{
public:
	RendererDevice(): width_(0), height_(0)
	{
	}

	virtual ~RendererDevice(){}

	virtual void CreateDevice() = 0;

	virtual void Resize(int width, int height)
	{
		width_ = width;
		height_ = height;
	}

//	virtual void DrawPoint(int x, int y, uint32_t color) = 0;

	virtual void RenderToScreen() = 0;
	

protected:
	int width_;
	int height_;
};

#ifdef _WIN32
#include <windows.h>
class GDIDevice: public RendererDevice
{
public:
	GDIDevice();
	~GDIDevice() override;
	void Resize(int width, int height) override;
	void DrawPoint(int x, int y, uint32_t color);

	void CreateDevice() override
	{
	}

	void RenderToScreen() override;
protected:
	uint32_t* buffer_;
	HDC hDC;
	HDC Memhdc;
	HBITMAP Membitmap;
	HBITMAP now_bitmap;

	void Release();

	LARGE_INTEGER t0, t1, tf;
};

#endif // _WIN32