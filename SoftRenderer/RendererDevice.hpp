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

	virtual void DrawPoint(int x, int y, uint32_t color) = 0;

	virtual void RenderToScreen(LPWSTR) = 0;
	

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
	void DrawPoint(int x, int y, uint32_t color) override;

	void CreateDevice() override
	{
	}

	void RenderToScreen(LPWSTR) override;
protected:
	uint32_t* buffer_;
	HDC hDC;
	HDC Memhdc;
	HBITMAP Membitmap;
	HBITMAP now_bitmap;

	void Release();
};

#endif // _WIN32