// Enums and macros for CRegexAssistantDlg
#pragma once
enum Regex_Compatibility
{
	//ToDo: Future Compatibility menu:
	REGEX_COMPATIBILITY_SCINTILLA_POSIX,
	REGEX_COMPATIBILITY_ALL,				        // Do conversions with boost all,
	REGEX_COMPATIBILITY_BOOST,						// Use boost default,
	REGEX_COMPATIBILITY_BOOST_DEFAULT_MULTILINE,	   
	REGEX_COMPATIBILITY_BOOST_ALL_STANDARD,			   
	REGEX_COMPATIBILITY_BOOST_ALL_MULTILINE,		   
	REGEX_COMPATIBILITY_BOOST_PERL_MULTILINE,		   
	REGEX_COMPATIBILITY_ECMAScript,					// Use C++ STD Regex ECMAScript,
	REGEX_COMPATIBILITY_CPP_BASIC,				    // Use C++ STD Regex Basic,
	REGEX_COMPATIBILITY_CPP_EXT,				    // Use C++ STD Regex Extended,
	REGEX_COMPATIBILITY_GROOVY,						// Use Java regex,
	REGEX_COMPATIBILITY_JAVA,						// Try implementing java code in this application, or do conversions with boost all,
	REGEX_COMPATIBILITY_JAVASCRIPT,				    // Use boost Perl,
	REGEX_COMPATIBILITY_DOTNET,					    // Try to include .net implementation for regex,
	REGEX_COMPATIBILITY_PCRE2,						// C++ PCRE2,
	REGEX_COMPATIBILITY_PERL,						// SAME AS TCL -- Use boost perl,
	REGEX_COMPATIBILITY_PHP,						// Use with PCRE2, but add conversion for PHP search are replace syntax,
	REGEX_COMPATIBILITY_POSIX_ERE, 				    // This is POSIX-ERE, DO CONVERSIONS ON POSIX CLASS with Scentilla POSIX,
	REGEX_COMPATIBILITY_POSIX_BRE,	    			// Use default Scentilla (No conversions needed),
	REGEX_COMPATIBILITY_POWERSHELL,					// Use .net implementation,
	REGEX_COMPATIBILITY_PYTHON,						// Try to include python code to do the regex,
	REGEX_COMPATIBILITY_R,							// Use with PCRE2, but add conversion for R replacement syntax,
	REGEX_COMPATIBILITY_RE2,						// Use RE2 library,
	REGEX_COMPATIBILITY_RUBY,						// use boost perl, plus added conversion,
	REGEX_COMPATIBILITY_Scintilla,					// Use default Scintilla,
	REGEX_COMPATIBILITY_SCINTILLAPOSIX,
	REGEX_COMPATIBILITY_SCINTILLA_CPPREGEX,
	REGEX_COMPATIBILITY_SED,						// Use Boost SED,
	REGEX_COMPATIBILITY_CPP_REGEX_SED,				   
	REGEX_COMPATIBILITY_TCL,						// SAME AS PERL -- Use boost perl,
	REGEX_COMPATIBILITY_VBSCRIPT,					// Same as javascript,
	REGEX_COMPATIBILITY_VIM,						// Do conversions with boost all
													   
	//REGEX_COMPATIBILITY_COMMON,				         //Use boost all,
	//REGEX_COMPATIBILITY_BOOST_PERL_STANDARD,		   
	//REGEX_COMPATIBILITY_UNIX_OLD,					// Use Sentilla default,
//#ifdef _DEBUG										   
//	REGEX_COMPATIBILITY_DUMMY1,						// Dummy enums for testing purposes
//	REGEX_COMPATIBILITY_DUMMY2,						// Dummy enums for testing purposes
//	REGEX_COMPATIBILITY_DUMMY3,						// Dummy enums for testing purposes
//	REGEX_COMPATIBILITY_DUMMY4,						// Dummy enums for testing purposes
//	REGEX_COMPATIBILITY_DUMMY5,						// Dummy enums for testing purposes
//#endif												   
													   
	REGEX_COMPATIBILITY_Z						    // Place holder
};													   
													   
enum
{
	RGB_Lime,
	RGB_YELLOW,
	RGB_Cyan,
	RGB_Orange,
	RGB_Pink,
	NUM_OF_MARKERS,

	REGEX_SUBSET_SCINTILLA = 128,		//See https://www.scintilla.org/SciTERegEx.html
	REGEX_SUBSET_BOOST = 256,
	REGEX_SUBSET_BOOST_ALL = 512,
	REGEX_SUBSET_STD_REGEX = 2048,
	REGEX_SUBSET_PCRE2 = 4096,
	REGEX_SUBSET_RE2 = 8192,
	REGEX_SUBSET_DOTNET = 16384,
	REGEX_SUBSET_PYTHON = 32768,
	REGEX_SUBSET_JAVA = 65536,

	REGEX_SUBSET_UNIX_OLD_SYNTAX = REGEX_SUBSET_JAVA * 2,
	REGEX_SUBSET_PERL = REGEX_SUBSET_UNIX_OLD_SYNTAX * 2,
	REGEX_SUBSET_MULTILINE = REGEX_SUBSET_PERL * 2,
	REGEX_SUBSET_SED = REGEX_SUBSET_MULTILINE * 2,
	REGEX_SUBSET_POSIX = REGEX_SUBSET_SED * 2,

	REGEX_SUBSET_DOLLAR_PREFIX_ONLY = REGEX_SUBSET_POSIX * 2,
	REGEX_SUBSET_BACKSLASH_PREFIX_ONLY = REGEX_SUBSET_DOLLAR_PREFIX_ONLY * 2,
	REGEX_SUBSET_STD_REGEX_EXT = REGEX_SUBSET_BACKSLASH_PREFIX_ONLY * 2,
	REGEX_SUBSET_STD_REGEX_BASIC = REGEX_SUBSET_STD_REGEX_EXT * 2,
};

enum SampleLoadMethod { SampleLoadDefault, SampleLoadFromCommandLine, SampleLoadFromFile, SampleLoadFromClipboard };



