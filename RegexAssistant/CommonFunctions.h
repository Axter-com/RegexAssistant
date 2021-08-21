#pragma once

#include <string>
#include <cstring>
#include <mutex>

enum { QISUCCESS = 0};
#define LOGEXT	"log"

class auto_lock
{
	std::mutex mtx;
public:
	auto_lock() { mtx.lock(); }
	~auto_lock() { mtx.unlock(); }
};

namespace FXString
{
	std::wstring ToWString(std::string src);
	inline std::wstring ToWString( const std::wstring &src ) { return src; }
	inline std::wstring ToWString( const CString &src) { return src.operator LPCWSTR(); }

	inline CString ToTString(const char * src) { return ToWString(src).c_str(); }
	inline CString ToTString(const wchar_t * src) { return src; }

	std::string ToString( std::wstring src);
	inline std::string ToString( const std::string &src ) { return src; }
	//inline std::string ToString( const CString &src ) { return ToString( std::wstring(src.operator LPCWSTR()) ); }

	std::string CopyAndReplaceAll(std::string src, const std::string &searchStr, const std::string &ReplacementStr);
	void ReplaceAll(std::string &src, const std::string &searchStr, const std::string &ReplacementStr);

	template <class strType1, class strType2> bool MatchesNoCase(strType1 str1, strType2 str2)
	{
		std::wstring wstr1 = ToWString(str1);
		std::wstring wstr2 = ToWString(str2);
		return (0 == _wcsicmp(wstr1.c_str(), wstr2.c_str()));
	}

	inline size_t StrLen(const char * str) { return strlen(str); }
	inline size_t StrLen(const wchar_t * str) { return wcslen(str); }
	inline size_t StrLen(const CString &str) { return static_cast <size_t>(str.GetLength()); }

	inline DWORD StrDwLen( const char * str ) { return static_cast<DWORD>(strlen( str )); }
	inline DWORD StrDwLen( const wchar_t * str ) { return static_cast<DWORD>(wcslen( str )); }
	inline DWORD StrDwLen( const CString &str ) { return static_cast<DWORD>(str.GetLength()); }
}


