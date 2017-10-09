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

#define DEF_HAS_MEMBER(m)														\
template <typename T>															\
struct has_member_##m{															\
	template <typename _T>														\
	static auto check(_T) -> decltype(_T::m) {};								\
	static void check(...){};		\
	using type = decltype(check(std::declval<T>()));							\
	static constexpr bool value = !std::is_void<type>::value;					\
};

#define HAS_MEMBER(T, m)	(has_member_##m<T>::value)
#define MEMBER_TYPE(T, m)	(has_member_##m<T>::type)

static bool fast_judge(const float& a)
{
//	byte *t = (byte*)(&a);
//#if _DEBUG
//	byte debug[4];
//	memcpy(debug, t, 8);
//#endif //_DEBUG
//	// < 0
//	if ((t[3] & 0x80) == 0x80) return false;
//	int exp = (t[3] & 0x7F) << 1 | ((t[2] >> 7) & 0x01);
//	// > 1
//	if (exp > 255) return false;
//	return true;
//	int i = *(int*)&a;
//	auto exp = (i >> 23) & 0xFF;
//	bool sign = ((i >> 31) & 0x01);
//	return (sign || exp >= 127);
	return !(a > 0 && a < 1);
}