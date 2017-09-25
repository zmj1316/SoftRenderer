#pragma once
#include <windows.h>
#include <vector>

class Renderer
{
public:
	Renderer();
	~Renderer();

	void CreateDevice();
	void Resize(int width, int height);

protected:
	HDC hDC;
	HDC Memhdc;
	HBITMAP Membitmap;
	HBITMAP now_bitmap;

	std::vector<uint32_t> buffer;
};

