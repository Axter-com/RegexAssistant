/*
	Copyright (C) 2021  David Maisonave
	The RegexAssistant source code is free software. You can redistribute it and/or modify it under the terms of the GNU General Public License.
	This program is distributed in the hope that it will be useful,	but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
*/
#pragma once
#include <string>
#include <memory>
#include <mutex>


namespace Common
{
	std::wstring ToWString(std::string src);
	inline std::wstring ToWString( const std::wstring &src ) { return src; }
	inline std::wstring ToWString( const LPCWSTR src ) { return std::wstring( src ); }

	std::string ToString( std::wstring src);
	inline std::string ToString( const std::string &src ) { return src; }
	inline std::string ToString( const LPCWSTR src ) { return ToString( std::wstring(src) ); }

	inline CString ToCString(const char * src) { return ToWString(src).c_str(); }
	inline CString ToCString(const wchar_t * src) { return src; }

	inline std::string ToString( int src ) { return std::to_string( src ); }
	inline std::string ToString( long src ) { return std::to_string( src ); }
	inline std::string ToString( long long src ) { return std::to_string( src ); }
	inline std::string ToString( unsigned src ) { return std::to_string( src ); }
	inline std::string ToString( unsigned long src ) { return std::to_string( src ); }
	inline std::string ToString( unsigned long long src ) { return std::to_string( src ); }
	inline std::string ToString( float src ) { return std::to_string( src ); }
	inline std::string ToString( double src ) { return std::to_string( src ); }
	inline std::string ToString( long double src ) { return std::to_string( src ); }

	inline std::wstring ToWString( int src ) { return std::to_wstring( src ); }
	inline std::wstring ToWString( long src ) { return std::to_wstring( src ); }
	inline std::wstring ToWString( long long src ) { return std::to_wstring( src ); }
	inline std::wstring ToWString( unsigned src ) { return std::to_wstring( src ); }
	inline std::wstring ToWString( unsigned long src ) { return std::to_wstring( src ); }
	inline std::wstring ToWString( unsigned long long src ) { return std::to_wstring( src ); }
	inline std::wstring ToWString( float src ) { return std::to_wstring( src ); }
	inline std::wstring ToWString( double src ) { return std::to_wstring( src ); }
	inline std::wstring ToWString( long double src ) { return std::to_wstring( src ); }

	inline CString ToCString( int src ) { return std::to_wstring( src ).c_str(); }
	inline CString ToCString( long src ) { return std::to_wstring( src ).c_str(); }
	inline CString ToCString( long long src ) { return std::to_wstring( src ).c_str(); }
	inline CString ToCString( unsigned src ) { return std::to_wstring( src ).c_str(); }
	inline CString ToCString( unsigned long src ) { return std::to_wstring( src ).c_str(); }
	inline CString ToCString( unsigned long long src ) { return std::to_wstring( src ).c_str(); }
	inline CString ToCString( float src ) { return std::to_wstring( src ).c_str(); }
	inline CString ToCString( double src ) { return std::to_wstring( src ).c_str(); }
	inline CString ToCString( long double src ) { return std::to_wstring( src ).c_str(); }

	std::string ReplaceInCopy(const std::string &src, const std::string &searchStr, const std::string &ReplacementStr);
	std::string Replace(std::string &src, const std::string &searchStr, const std::string &ReplacementStr);
	void ReplaceAll( std::string &src, const std::string &searchStr, const std::string &ReplacementStr );

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


	class errno_exception
	{
		int m_errno;
		std::string m_what;
		std::exception *m_exception;
		std::string m_name;
	public:
		errno_exception( const char *name, std::exception &Exception, int no = GetLastError(), std::string s = "" ) :m_errno( no ), m_what( s.size() ? s : Exception.what() ), m_exception( &Exception ), m_name( name ) {}
		errno_exception( const char *s = NULL, int no = GetLastError() ) :m_errno( no ), m_what( (s == NULL) ? "Unknown Exception" : s ), m_exception( NULL ), m_name( (s == NULL) ? "Unknown Exception" : "Exception" ) {}
		errno_exception( std::string &s, int no = GetLastError() ) :m_errno( no ), m_what( s ), m_exception( NULL ), m_name( "Exception" ) {}
		const char* what() { return (m_exception == NULL) ? m_what.c_str() : m_exception->what(); }
		int code() { return m_errno; }
		const char* name() { return m_name.c_str(); }
	};

	class auto_lock
	{
		std::mutex mtx;
	public:
		auto_lock(){ mtx.lock(); }
		~auto_lock(){ mtx.unlock(); }
	};

	void TraceDiagnosticInfo( std::string msg );

//
//	enum Attrib
//	{
//#ifdef _DEBUG
//		DEBUG_ON = 1 << 31,
//#else
//		DEBUG_ON = 0,
//#endif
//		ONFIRSTPASS = 1 << 18,							// Log, Popup, or trace only on first pass
//		LOG_ON_EXIT = 1 << 19,							// Postpone logging until exit from scope
//
//		LOG_REL = 1 << 26,							// Log only in release mode.  Use in conjuntion with INF, WRN, ERR, & FATAL
//		LOG_DBG = 1 << 27,							// Log only in debug mode.  Use in conjuntion with INF, WRN, ERR, & FATAL
//		POP_REL = 1 << 28,							// Popup or Modeless only in release mode
//		POP_DBG = 1 << 29,							// Popup or Modeless only in debug mode
//
//		TIMEOUT10Sec = 10,									// Have a 10 second time-out for MODELESS or POPUPT
//		TIMEOUT30Sec = 30,									// Have a 30 second time-out for MODELESS or POPUPT
//		TIMEOUT1Min = 60,									// Have a 1 minute time-out for MODELESS or POPUPT
//		TIMEOUT2Min = 120,								// Have a 2 minutes time-out for MODELESS or POPUPT
//		TIMEOUT5Min = 300,								// Have a 5 minutes time-out for MODELESS or POPUPT
//		TIMEOUT10Min = 600,								// Have a 10 minutes time-out for MODELESS or POPUPT
//		TIMEOUT15Min = 900,								// Have a 15 minutes time-out for MODELESS or POPUPT
//
//	};
//
//	enum LogType
//	{
//		TRACEMSG = 1 << 20,							// Trace message (only in debug mode)
//		LOGMSG = 1 << 21,							// Log to a file
//		STDCERR = 1 << 22,							// Log to std::cerr
//		POPUP = 1 << 23,							// Call MessageBox with logging details
//		MODELESS = 1 << 24,							// Create a modeless window that will close automatically via time-out
//		POPUPT = 1 << 25,							// Call MessageBox which will close automatically via time-out
//	};
//
//	enum Verbosity
//	{
//		//Verbosity Level
//		OFF = 1 << 12,							// Logging off
//		DBG = 1 << 13,							// Debug logging
//		INF = 1 << 14,							// Informational logging
//		WRN = 1 << 15,							// An unexpected result occurred, but the program can continue safely
//		ERR = 1 << 16,							// An unexpected result occurred, which stops the program from completing a required task
//		FATAL = 1 << 17,							// An error occurred, in which the program can not continue, and must exit
//	};
//
//	class Logging
//	{
//	public:
//		Logging( const char* SoureCode_FileName, int SourceCode_LineNo, const char* SoureCode_FunctionName, Verbosity verbosity, const wchar_t* pszFormat, CString s1 );
//		Logging( const char* SoureCode_FileName, int SourceCode_LineNo, const char* SoureCode_FunctionName, Verbosity verbosity, const wchar_t* pszFormat, CString s1, CString s2 );
//		//Logging( const char* SoureCode_FileName, int SourceCode_LineNo, const char* SoureCode_FunctionName, Verbosity verbosity, const wchar_t* pszFormat, va_list argList );
//		//Logging( const char* SoureCode_FileName, int SourceCode_LineNo, const char* SoureCode_FunctionName, Verbosity verbosity, const char* pszFormat, va_list argList );
//		static int GetGlobalLoggingSettings();
//		static int AddAttrib( int logattrib );
//		static int RemoveAttrib( int logattrib );
//		static int SetAttrib( int logattrib );
//	protected:
//		static int GlobalLoggingSettings;
//	};
}

#define REPORT_ERR_AND_EXIT(exit_err_no, msg_format, ...) {CString msg; msg.Format(_T("Error:  " msg_format "  Performming early exit due to error."),__VA_ARGS__);AfxMessageBox(msg);exit(exit_err_no);}

#define ERR_LOG(msg_format, ...	)	{Common::Logging logging(__FILE__, __LINE__, __FUNCTION__, Common::Verbosity::ERR, msg_format __VA_OPT__(,) __VA_ARGS__);}

