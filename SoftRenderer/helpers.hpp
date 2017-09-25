#pragma once
#include <windows.h>
#include <tchar.h>     
#include <stdio.h>     
#include <assert.h>

inline int CDECL MessageBoxPrintf(WCHAR* szFormat, ...)
{
	WCHAR   szBuffer[1024];
	va_list pArgList;

	va_start(pArgList, szFormat);

	_vsntprintf_s(szBuffer, sizeof(szBuffer) / sizeof(TCHAR),
		_TRUNCATE, szFormat, pArgList);

	va_end(pArgList);

	return MessageBox(NULL, szBuffer, _T("Info"), MB_OK);
}

#define LOG_INFO(format, ...) MessageBoxPrintf(format,##__VA_ARGS__)

#define MY_ASSERT(exp) if(!exp) LOG_INFO(L"assert %s failed at %s line %d", _CRT_WIDE(#exp) , _CRT_WIDE(__FILE__), __LINE__), assert(false)
