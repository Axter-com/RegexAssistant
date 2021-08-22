// RegexAssistant.h : main header file for the PROJECT_NAME application
#pragma once
#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif
#include "resource.h"		// main symbols


enum Regex_Compatibility
{
	REGEX_COMPATIBILITY_BOOST,
	REGEX_COMPATIBILITY_SCINTILLA_POSIX,
	REGEX_COMPATIBILITY_SCINTILLA,
	REGEX_COMPATIBILITY_BOOST_MULTILINE,
	REGEX_COMPATIBILITY_BOOST_PERL,
	REGEX_COMPATIBILITY_BOOST_SED,
	REGEX_COMPATIBILITY_BOOST_DEFAULT,
	REGEX_COMPATIBILITY_STD_REGEX,
	REGEX_COMPATIBILITY_STD_REGEX_SED,
	NUM_OF_MARKERS = 10
};

class CRegexAssistantApp : public CWinApp
{
public:
	CRegexAssistantApp();
	enum SampleLoadMethod {SampleLoadDefault, SampleLoadFromCommandLine, SampleLoadFromFile, SampleLoadFromClipboard	};
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	DECLARE_MESSAGE_MAP()
};

extern CRegexAssistantApp theApp;
