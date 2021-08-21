#include "pch.h"
#include "CommonFunctions.h"
#include <string>
#include <codecvt>
#include <locale>
#include <boost/algorithm/string/replace.hpp>

namespace FXString
{
	std::wstring ToWString(std::string src)	{
		return std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(src);
	}

	std::string ToString(std::wstring src )	{
		return std::wstring_convert<std::codecvt_utf8<wchar_t>>().to_bytes(src);
	}

	void ReplaceAll(std::string &str, const std::string &searchStr, const std::string &ReplacementStr)	{
		boost::replace_all(str, searchStr, ReplacementStr);
	}

	std::string CopyAndReplaceAll(std::string str, const std::string &searchStr, const std::string &ReplacementStr)
	{
		ReplaceAll(str, searchStr, ReplacementStr);
		return str;
	}
}

