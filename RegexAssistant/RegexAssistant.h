/*
	Copyright (C) 2021  David Maisonave
	The RegexAssistant source code is free software. You can redistribute it and/or modify it under the terms of the GNU General Public License.
	This program is distributed in the hope that it will be useful,	but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
*/
#pragma once
#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif
#include "resource.h"		// main symbols
#include <string>

enum Regex_Compatibility
{
	//ToDo: Future Compatibility menu:
	REGEX_COMPATIBILITY_SCINTILLA_POSIX,
	REGEX_COMPATIBILITY_ALL,				        //Do conversions with boost all,
	REGEX_COMPATIBILITY_BOOST,						//Use boost default,
	REGEX_COMPATIBILITY_BOOST_DEFAULT_MULTILINE,
	REGEX_COMPATIBILITY_BOOST_ALL_STANDARD,
	REGEX_COMPATIBILITY_BOOST_ALL_MULTILINE,
	REGEX_COMPATIBILITY_BOOST_PERL_MULTILINE,
	REGEX_COMPATIBILITY_CPP,					    //Use C++ Regex default,
	REGEX_COMPATIBILITY_ECMAScript,					//Use boost all,
	REGEX_COMPATIBILITY_GROOVY,						//Use Java regex,
	REGEX_COMPATIBILITY_JAVA,						//Try implementing java code in this application, or do conversions with boost all,
	REGEX_COMPATIBILITY_JAVASCRIPT,				    //Use boost Perl,
	REGEX_COMPATIBILITY_DOTNET,					    //Try to include .net implementation for regex,
	REGEX_COMPATIBILITY_PCRE2,						//C++ PCRE2,
	REGEX_COMPATIBILITY_PERL,						//SAME AS TCL -- Use boost perl,
	REGEX_COMPATIBILITY_PHP,						//Use with PCRE2, but add conversion for PHP search are replace syntax,
	REGEX_COMPATIBILITY_POSIX_ERE, 				    //This is POSIX-ERE, DO CONVERSIONS ON POSIX CLASS with Scentilla POSIX,
	REGEX_COMPATIBILITY_POSIX_BRE,	    			//Use default Scentilla (No conversions needed),
	REGEX_COMPATIBILITY_POWERSHELL,					//Use .net implementation,
	REGEX_COMPATIBILITY_PYTHON,						//Try to include python code to do the regex,
	REGEX_COMPATIBILITY_R,							//Use with PCRE2, but add conversion for R replacement syntax,
	REGEX_COMPATIBILITY_RE2,						//Use RE2 library,
	REGEX_COMPATIBILITY_RUBY,						//use boost perl, plus added conversion,
	REGEX_COMPATIBILITY_Scintilla,					//Use default Scintilla,
	REGEX_COMPATIBILITY_SCINTILLAPOSIX,
	REGEX_COMPATIBILITY_SED,						//Use Boost SED,
	REGEX_COMPATIBILITY_CPP_REGEX_SED,
	REGEX_COMPATIBILITY_TCL,						//SAME AS PERL -- Use boost perl,
	REGEX_COMPATIBILITY_VBSCRIPT,					//Same as javascript,
	REGEX_COMPATIBILITY_VIM,						//Do conversions with boost all

	//REGEX_COMPATIBILITY_COMMON,				        //Use boost all,
	//REGEX_COMPATIBILITY_BOOST_PERL_STANDARD,
	//REGEX_COMPATIBILITY_UNIX_OLD,					//Use Sentilla default,
#ifdef _DEBUG
	REGEX_COMPATIBILITY_DUMMY1,						//Dummy enums for testing purposes
	REGEX_COMPATIBILITY_DUMMY2,						//Dummy enums for testing purposes
	REGEX_COMPATIBILITY_DUMMY3,						//Dummy enums for testing purposes
	REGEX_COMPATIBILITY_DUMMY4,						//Dummy enums for testing purposes
	REGEX_COMPATIBILITY_DUMMY5,						//Dummy enums for testing purposes
#endif

	REGEX_COMPATIBILITY_Z						    //Place holder
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
