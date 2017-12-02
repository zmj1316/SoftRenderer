#include <locale.h>
#include <stdio.h>
#include <string>
#include <wchar.h>
#include <Windows.h>
#include <memory>
#include <vector>

namespace oguna
{
	/// CP932,UTF8,UTF16‚ð‘ŠŒÝ•ÏŠ·‚·‚é
	class EncodingConverter
	{
	public:
#define MAX_BUF 2048

		static std::wstring sjis2utf16(const std::string &sjis)
		{
			LPCCH pSJIS = (LPCCH)sjis.c_str();
			std::wstring result;
			int utf16size = ::MultiByteToWideChar(932, MB_ERR_INVALID_CHARS, pSJIS, -1, 0, 0);
			if (utf16size != 0)
			{
				LPWSTR pUTF16 = new WCHAR[utf16size];
				if (::MultiByteToWideChar(932, 0, (LPCCH)pSJIS, -1, pUTF16, utf16size) != 0)
				{
					result = std::wstring(pUTF16);
				}
			}
			return result;
		}


	static std::string sjis2utf8(const std::string &sjis)
	{
		std::string utf8_string;

		//Ò»µ©SJIS¤«¤éutf-16¤Ø‰ä“Q
		LPCCH pSJIS = (LPCCH)sjis.c_str();
		int utf16size = ::MultiByteToWideChar(CP_ACP, MB_ERR_INVALID_CHARS, pSJIS, -1, 0, 0);
		if (utf16size != 0)
		{
			LPWSTR pUTF16 = new WCHAR[utf16size];
			if (::MultiByteToWideChar(CP_ACP, 0, (LPCCH)pSJIS, -1, pUTF16, utf16size) != 0)
			{
				//´Î¤Ëutf-16¤«¤éutf-8¤Ø‰ä“Q
				int utf8size = ::WideCharToMultiByte(CP_UTF8, 0, pUTF16, -1, 0, 0, 0, 0);
				if (utf8size != 0)
				{
					LPSTR pUTF8 = new CHAR[utf8size + 16];
					ZeroMemory(pUTF8, utf8size + 16);
					if (::WideCharToMultiByte(CP_UTF8, 0, pUTF16, -1, pUTF8, utf8size, 0, 0) != 0)
					{
						utf8_string = std::string(pUTF8);
					}
					delete pUTF8;
				}
			}
			delete pUTF16;
		}
		return utf8_string;
	}

	/// UTF8‚©‚çCP932(std::string)‚Ö•ÏŠ·‚·‚é
	static int Utf8ToCp932(const char *src, int size, std::string *out)
	{
		std::wstring unicode;
		Utf8ToUtf16(src, size, &unicode);
		return Utf16ToCp932(unicode.data(), unicode.length(), out);
	}

	/// CP932‚©‚çUTF8(std::string)‚Ö•ÏŠ·‚·‚é
	static int Cp932ToUtf8(const char *src, int length, std::string *out)
	{
		std::wstring unicode;
		Cp932ToUtf16(src, length, &unicode);
		return Utf16ToUtf8(unicode.c_str(), unicode.length(), out);
	}

	/// CP932‚©‚çUTF16(std::wstring)‚Ö•ÏŠ·‚·‚é
	static int Cp932ToUtf16(const char *src, int length, std::wstring *out)
	{
		int size;
		std::vector<char> buffer;
		size = ::MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, src, length, NULL, NULL);
		buffer.resize(size * sizeof(wchar_t) * 2);
		MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, src, length, (LPWSTR) buffer.data(), buffer.size() * 2);
		out->assign((wchar_t*) buffer.data(), size);
		return size;
	}

	/// UTF16‚©‚çCP932(std::string)‚Ö•ÏŠ·‚·‚é
	static int Utf16ToCp932(const wchar_t *src, int length, std::string *out)
	{
		int size;
		std::vector<char> buffer;
		size = WideCharToMultiByte(CP_ACP, NULL, src, length, NULL, NULL, NULL, NULL);
		buffer.resize(size);
		WideCharToMultiByte(CP_ACP, NULL, src, length, (LPSTR) buffer.data(), buffer.size(), NULL, NULL);
		out->assign(buffer.data(), size);
		return size;
	}

	/// UTF8‚©‚çUTF16(std::wstring)‚Ö•ÏŠ·‚·‚é
	static int Utf8ToUtf16(const char *src, int length, std::wstring *out)
	{
		int size;
		std::vector<char> buffer;

		size = ::MultiByteToWideChar(CP_UTF8, 0, src, length, NULL, NULL);
		buffer.resize(size * sizeof(wchar_t));
		MultiByteToWideChar(CP_UTF8, 0, src, length, (LPWSTR) buffer.data(), buffer.size());
		out->swap(std::wstring((wchar_t*) buffer.data(), size));
		return size;
	}

	/// UTF16‚©‚çUTF8(std::string)‚Ö•ÏŠ·‚·‚é
	static int Utf16ToUtf8(const wchar_t *src, int length, std::string *out)
	{
		int size;
		std::vector<char> buffer;

		size = WideCharToMultiByte(CP_UTF8, NULL, src, length, NULL, NULL, NULL, NULL);
		buffer.resize(size);
		WideCharToMultiByte(CP_UTF8, NULL, src, length, (LPSTR) buffer.data(), buffer.size(), NULL, NULL);
		out->assign(buffer.data(), size);
		return size;
	}
};

}
