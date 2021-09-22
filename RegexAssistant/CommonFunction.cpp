/*
	Copyright (C) 2021  David Maisonave
	The RegexAssistant source code is free software. You can redistribute it and/or modify it under the terms of the GNU General Public License.
	This program is distributed in the hope that it will be useful,	but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
*/
#include "pch.h"
#include "CommonFunctions.h"
#include <boost/algorithm/string/replace.hpp>



namespace Common
{
	std::wstring ToWString(std::string str )	
	{
		if ( str.empty() ) return std::wstring();
		int size_needed = MultiByteToWideChar( CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0 );
		std::wstring wstrTo( size_needed, 0 );
		MultiByteToWideChar( CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed );
		return wstrTo;
	}

	std::string ToString(std::wstring wstr )	
	{
		if ( wstr.empty() ) return std::string();
		int size_needed = WideCharToMultiByte( CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL );
		std::string strTo( size_needed, 0 );
		WideCharToMultiByte( CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL );
		return strTo;
	}

	void ReplaceAll( std::string &src, const std::string &searchStr, const std::string &ReplacementStr )
	{
		boost::replace_all( src, searchStr, ReplacementStr );
	}

	std::string Replace( std::string &src, const std::string &searchStr, const std::string &ReplacementStr )
	{
		ReplaceAll( src, searchStr, ReplacementStr );
		return src;
	}

	std::string ReplaceInCopy(const std::string &org, const std::string &searchStr, const std::string &ReplacementStr)
	{
		std::string str = org;
		ReplaceAll(str, searchStr, ReplacementStr);
		return str;
	}


	void TraceDiagnosticInfo( std::string msg )
	{// AfxTrace has a max buffer size of 512 bytes, which is 256 wchar_t's.  Need to make sure the diagnostic info doesn't overrun the buffer by sending it MaxSectionSize characters at a time
		const int MaxSectionSize = 100;
		const int strsize = msg.size();
		if ( strsize > MaxSectionSize )
		{
			Common::Replace( msg, "\n", " " );
			Common::Replace( msg, "\r", " " );
		}
		for ( int i = 0; (i * MaxSectionSize) < strsize; ++i )
			AfxTrace( i ? L"\t\t\t\t%S\n" : L"Exception Info:\t%S\n", msg.substr( i * MaxSectionSize, (i + 1) * MaxSectionSize ).c_str() );
	}

	//Logging::Logging( const char* SoureCode_FileName, int SourceCode_LineNo, const char* SoureCode_FunctionName, Verbosity verbosity, const wchar_t* pszFormat, CString s1 )
	//{
	//	CString strVerbosity;
	//	switch ( verbosity )
	//	{
	//	case DBG:
	//		strVerbosity = _T( "Debug" );
	//		break;
	//	case INF:
	//		strVerbosity = _T( "Info" );
	//		break;
	//	case WRN:
	//		strVerbosity = _T( "Warn" );
	//		break;
	//	case ERR:
	//		strVerbosity = _T( "Error" );
	//		break;
	//	case FATAL:
	//		strVerbosity = _T( "Fatal-Error" );
	//		break;
	//	}
	//	CString Msg;
	//	Msg.Format( _T( "%S(%i):%S:\t%s:\t%s" ), SoureCode_FileName, SourceCode_LineNo, SoureCode_FunctionName, strVerbosity.GetBuffer(), pszFormat );
	//	const wchar_t * msg = Msg.GetBuffer();
	//	AfxTrace( msg, s1 );

	//}

	//Logging::Logging( const char* SoureCode_FileName, int SourceCode_LineNo, const char* SoureCode_FunctionName, Verbosity verbosity, const wchar_t* pszFormat, CString s1, CString s2 )
	//{
	//	CString strVerbosity;
	//	switch ( verbosity )
	//	{
	//	case DBG:
	//		strVerbosity = _T( "Debug" );
	//		break;
	//	case INF:
	//		strVerbosity = _T( "Info" );
	//		break;
	//	case WRN:
	//		strVerbosity = _T( "Warn" );
	//		break;
	//	case ERR:
	//		strVerbosity = _T( "Error" );
	//		break;
	//	case FATAL:
	//		strVerbosity = _T( "Fatal-Error" );
	//		break;
	//	}
	//	CString Msg;
	//	Msg.Format( _T( "%S(%i):%S:\t%s:\t%s" ), SoureCode_FileName, SourceCode_LineNo, SoureCode_FunctionName, strVerbosity.operator LPCWSTR(), pszFormat );
	//	AfxTrace( Msg, s1, s2 );

	//}

	//Logging::Logging( const char* SoureCode_FileName, int SourceCode_LineNo, const char* SoureCode_FunctionName, Verbosity verbosity, const wchar_t* pszFormat, va_list argList )
	//{
	//	CString strVerbosity;
	//	switch ( verbosity )
	//	{
	//	case DBG:
	//		strVerbosity = _T( "Debug" );
	//		break;
	//	case INF:
	//		strVerbosity = _T( "Info" );
	//		break;
	//	case WRN:
	//		strVerbosity = _T( "Warn" );
	//		break;
	//	case ERR:
	//		strVerbosity = _T( "Error" );
	//		break;
	//	case FATAL:
	//		strVerbosity = _T( "Fatal-Error" );
	//		break;
	//	}
	//	CString Msg;
	//	Msg.Format(_T( "%S(%i):%S:\t%s:\t%s" ), SoureCode_FileName , SourceCode_LineNo , SoureCode_FunctionName , strVerbosity.operator LPCWSTR(), pszFormat);
	//	AfxTrace( Msg, argList );
	//}

	//Logging::Logging( const char* SoureCode_FileName, int SourceCode_LineNo, const char* SoureCode_FunctionName, Verbosity verbosity, const char* pszFormat, va_list argList )
	//{
	//	CString strVerbosity;
	//	switch ( verbosity )
	//	{
	//	case DBG:
	//		strVerbosity = _T( "Debug" );
	//		break;
	//	case INF:
	//		strVerbosity = _T( "Info" );
	//		break;
	//	case WRN:
	//		strVerbosity = _T( "Warn" );
	//		break;
	//	case ERR:
	//		strVerbosity = _T( "Error" );
	//		break;
	//	case FATAL:
	//		strVerbosity = _T( "Fatal-Error" );
	//		break;
	//	}
	//	CString Msg;
	//	Msg.Format( _T( "%S(%i):%S:\t%s:\t%S" ), SoureCode_FileName, SourceCode_LineNo, SoureCode_FunctionName, strVerbosity.operator LPCWSTR(), pszFormat );
	//	AfxTrace( Msg, argList );
	//}

	//int Logging::GlobalLoggingSettings = Verbosity::WRN | Attrib::TIMEOUT30Sec;
	//
	//int Logging::GetGlobalLoggingSettings()
	//{
	//	return GlobalLoggingSettings;
	//}

	//int Logging::AddAttrib( int logattrib )
	//{
	//	GlobalLoggingSettings |= logattrib;
	//	return GlobalLoggingSettings;
	//}

	//int Logging::RemoveAttrib( int logattrib )
	//{
	//	GlobalLoggingSettings &= ~(1UL << logattrib);
	//	return GlobalLoggingSettings;
	//}

	//int Logging::SetAttrib( int logattrib )
	//{
	//	GlobalLoggingSettings = logattrib;
	//	return GlobalLoggingSettings;
	//}
}

