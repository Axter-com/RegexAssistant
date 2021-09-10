/*
	Copyright (C) 2021  David Maisonave
	The RegexAssistant source code is free software. You can redistribute it and/or modify it under the terms of the GNU General Public License.
	This program is distributed in the hope that it will be useful,	but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
*/
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

