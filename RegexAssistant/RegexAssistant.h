// RegexAssistant.h : main header file for the PROJECT_NAME application
#pragma once
#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif
#include "resource.h"		// main symbols

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
