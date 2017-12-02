#ifdef _WIN32
#include "helpers.hpp"
#include "RendererDevice.hpp"
#include "MYUT.hpp"


GDIDevice::GDIDevice(): buffer_(nullptr), hDC(nullptr), Memhdc(nullptr), Membitmap(nullptr), now_bitmap(nullptr)
{
}

GDIDevice::~GDIDevice()
{
	Release();
}


void GDIDevice::Resize(int width, int height)
{
	RendererDevice::Resize(width, height);
	Release();
	auto hWnd = MYUTGetHWND();

	hDC = GetDC(hWnd);

	Memhdc = CreateCompatibleDC(hDC);


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
	

	now_bitmap = CreateDIBSection(Memhdc, &bmp_info, DIB_RGB_COLORS, (void**)&buffer_, NULL, 0);
	QueryPerformanceFrequency(&tf);
	QueryPerformanceCounter(&t0);
	QueryPerformanceCounter(&t1);
}

void GDIDevice::DrawPoint(int x, int y, uint32_t color)
{
	MY_ASSERT( (x < width_) && (y < height_));
	buffer_[x + y * width_] = color;
}

void GDIDevice::RenderToScreen()
{

	QueryPerformanceFrequency(&tf);
	QueryPerformanceCounter(&t1);
	static double now_time = 0;
	now_time += 1.0 * (t1.QuadPart - t0.QuadPart) / tf.QuadPart;
	t0 = t1;
	static wchar_t tmp[100] = L"FPS:\0";
	static int count = 0;
	count++;
	if (now_time >= 1.0)
	{
		QueryPerformanceFrequency(&tf);
		now_time = 0;
		wsprintf(tmp, L"FPS: %d\0", count);
		count = 0;
	}
	// �������ֱ���ɫ
	SetBkColor(Memhdc, RGB(0, 0, 0));
	// ����������ɫ
	SetTextColor(Memhdc, RGB(255, 255, 255));
	TextOut(Memhdc, 20, 0, tmp, wcslen(tmp));
	SelectObject(Memhdc, now_bitmap);
	BitBlt(hDC, 0, 0, width_, height_, Memhdc, 0, 0, SRCCOPY);
	memset(buffer_, 0xFF02F456, sizeof(int) * width_ * height_);
}

void GDIDevice::Release()
{
	//auto hWnd = MYUTGetHWND();
	//if (hDC != nullptr)
	//{
	//	ReleaseDC(hWnd, hDC);
	//	hDC = nullptr;
	//}
	if (Memhdc != nullptr)
	{
		DeleteDC(Memhdc);
		Memhdc = nullptr;
	}
	if (Membitmap != nullptr)
	{
		DeleteObject(Membitmap);
		Membitmap = nullptr;
	}
	if (now_bitmap != nullptr)
	{
		DeleteObject(now_bitmap);
		now_bitmap = nullptr;
	}
}


#endif //_WIN32
