#pragma once
#include "helpers.hpp"
#include <windows.h>

HRESULT WINAPI MYUTCreateWindow(const WCHAR* strWindowTitle = L"MYUT Windows", HINSTANCE hInstance = nullptr,
	HICON hIcon = nullptr, HMENU hMenu = nullptr, int x = 444, int y = 444);

HRESULT WINAPI MYUTMainLoop(HACCEL hAccel = NULL);


#ifndef SAFE_DELETE
#define SAFE_DELETE(p)       { if (p) { delete (p);     (p)=NULL; } }
#endif


typedef LRESULT(CALLBACK *LPMYUTCALLBACKMSGPROC)(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool* pbNoFurtherProcessing, void* pUserContext);
typedef LRESULT(CALLBACK *LPMYUTCALLBACKDRAW)();
typedef LRESULT(CALLBACK *LPMYUTCALLBACKRESIZE)(int width, int height);


HWND WINAPI MYUTGetHWND();
HWND WINAPI MYUTGetHWNDFocus();
void WINAPI MYUTSetCallBackDraw(LPMYUTCALLBACKDRAW); 
void WINAPI MYUTSetCallBackResize(LPMYUTCALLBACKRESIZE);

bool* WINAPI MYUTGetKeys();