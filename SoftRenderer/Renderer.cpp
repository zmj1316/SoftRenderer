#include "Renderer.hpp"
#include "MYUT.hpp"


Renderer::Renderer(): hDC(nullptr), Memhdc(nullptr), Membitmap(nullptr), now_bitmap(nullptr)
{
}


Renderer::~Renderer()
{
}

void Renderer::CreateDevice()
{

}

void Renderer::Resize(int width, int height)
{
	auto hWnd = MYUTGetHWND();
	if (hDC != nullptr)
	{
		ReleaseDC(hWnd, hDC);
		hDC = nullptr;
	}
	hDC = GetDC(hWnd);
	if (Memhdc != nullptr)
	{
		DeleteDC(Memhdc);
		Memhdc = nullptr;
	}
	Memhdc = CreateCompatibleDC(hDC);

	if (Membitmap != nullptr)
	{
		DeleteObject(Membitmap);
		Membitmap = nullptr;
	}
	Membitmap = CreateCompatibleBitmap(hDC, width, height);
	SelectObject(Memhdc, Membitmap);

	BITMAPINFO bmp_info;
	bmp_info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);//�ṹ����ֽ���
	bmp_info.bmiHeader.biWidth = width;//������Ϊ��λ��λͼ��
	bmp_info.bmiHeader.biHeight = -height;//������Ϊ��λ��λͼ��,��Ϊ������ʾ�����Ͻ�Ϊԭ�㣬���������½�Ϊԭ��
	bmp_info.bmiHeader.biPlanes = 1;//Ŀ���豸��ƽ��������������Ϊ1
	bmp_info.bmiHeader.biBitCount = 32; //λͼ��ÿ�����ص�λ��
	bmp_info.bmiHeader.biCompression = BI_RGB;
	bmp_info.bmiHeader.biSizeImage = 0;
	bmp_info.bmiHeader.biXPelsPerMeter = 0;
	bmp_info.bmiHeader.biYPelsPerMeter = 0;
	bmp_info.bmiHeader.biClrUsed = 0;
	bmp_info.bmiHeader.biClrImportant = 0;

	buffer.resize(height * width);

	if(now_bitmap != nullptr)
	{
		DeleteObject(now_bitmap);
		now_bitmap = nullptr;
	}
	now_bitmap = CreateDIBSection(Memhdc, &bmp_info, DIB_RGB_COLORS, (void**)&buffer, NULL, 0);
}
