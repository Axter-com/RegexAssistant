//Note: This file should only be included in RegexAssistantDlg.cpp
//Initialize static members of CRegexAssistantDlg
const MARKERDATAtag CRegexAssistantDlg::m_MarkerData[] = {
	{RGB( 0, 0, 0 )/*Black*/, RGB( 0, 255, 0 )/*Lime*/, FALSE},       //(Green) For Boost
{RGB( 0, 0, 0 )/*Black*/, RGB( 255, 255, 0 )/*Yellow*/, FALSE},   // For REGEX_COMPATIBILITY_SCINTILLA_POSIX
{RGB( 0, 0, 0 )/*Black*/, RGB( 0, 255, 255 )/*Cyan*/, FALSE},     //For STD Regex and STD SED
{RGB( 0, 0, 0 /*Black*/ ), RGB( 255, 128, 0 )/*Orange*/, FALSE},  //For REGEX_COMPATIBILITY_SCINTILLA
{RGB( 0, 0, 0 /*Black*/ ), RGB( 254, 185, 170 )/*Pink*/, FALSE}  //For Java
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

	REGEX_SUBSET_UNIX_OLD_SYNTAX = 131072,
	REGEX_SUBSET_PERL = 262144,
	REGEX_SUBSET_MULTILINE = 524288,
	REGEX_SUBSET_SED = 1048576,
	REGEX_SUBSET_POSIX = 2097152
};

#define NOT_IMPLEMENTED FALSE, "", "Awaiting Implementation", 0

const RegexCompatibilityProperties CRegexAssistantDlg::m_RegexCompatibilityProperties[] =
{
	{_T( "POSIX" ), REGEX_COMPATIBILITY_SCINTILLA_POSIX, (REGEX_SUBSET_SCINTILLA | REGEX_SUBSET_POSIX), TRUE, "POSIX", "Scintilla POSIX syntax", RGB_YELLOW},
{_T( "All" ), REGEX_COMPATIBILITY_ALL, (REGEX_SUBSET_BOOST | REGEX_SUBSET_BOOST_ALL | REGEX_SUBSET_MULTILINE), NOT_IMPLEMENTED}, //Works with most regex syntax. In the backend, some syntax gets converted to work with Boost All
{_T( "Boost" ), REGEX_COMPATIBILITY_BOOST, REGEX_SUBSET_BOOST, TRUE, "Boost", "Default boost regex with flag regex_constants::format_default", RGB_Cyan},
{_T( "Boost (default) Multiline" ), REGEX_COMPATIBILITY_BOOST_DEFAULT_MULTILINE, (REGEX_SUBSET_BOOST | REGEX_SUBSET_MULTILINE), TRUE, "BoostDefaultMultiline", "Boost with multiline set", RGB_Cyan},
{_T( "Boost - All" ), REGEX_COMPATIBILITY_BOOST_ALL_STANDARD, (REGEX_SUBSET_BOOST | REGEX_SUBSET_BOOST_ALL), TRUE, "BoostAll", "Boost with flag regex_constants::format_all", RGB_Cyan},
{_T( "Boost - All Multiline" ), REGEX_COMPATIBILITY_BOOST_ALL_MULTILINE, (REGEX_SUBSET_BOOST | REGEX_SUBSET_BOOST_ALL | REGEX_SUBSET_MULTILINE), TRUE, "BoostAllMulti", "Boost with ALL (format_all) and multiline selected", RGB_Cyan},
{_T( "Boost Perl Multiline" ), REGEX_COMPATIBILITY_BOOST_PERL_MULTILINE, (REGEX_SUBSET_BOOST | REGEX_SUBSET_PERL | REGEX_SUBSET_MULTILINE), TRUE, "BoostPerl", "Boost with Perl (regex_constants::format_perl) and Multiline set", RGB_Cyan},
{_T( "C++ Regex" ), REGEX_COMPATIBILITY_CPP, REGEX_SUBSET_STD_REGEX, TRUE, "C++", "C++ STD Regex", RGB_Orange},
{_T( "ECMAScript" ), REGEX_COMPATIBILITY_ECMAScript, REGEX_SUBSET_STD_REGEX, TRUE, "ECMAScript", "ECMAScript via C++ Regex", RGB_Orange},
{_T( "Groovy" ), REGEX_COMPATIBILITY_GROOVY, REGEX_SUBSET_JAVA, NOT_IMPLEMENTED},// Same as Java
{_T( "Java" ), REGEX_COMPATIBILITY_JAVA, REGEX_SUBSET_JAVA, NOT_IMPLEMENTED},//Try implementing java code in this application, or do conversions with boost all
{_T( "JavaScript" ), REGEX_COMPATIBILITY_JAVASCRIPT, (REGEX_SUBSET_BOOST | REGEX_SUBSET_PERL), TRUE, "JavaScript", "JavaScript uses syntax similar to Perl.  Using Boost Perl (regex_constants::format_perl) to emulate JavaScript Regex.", RGB_Cyan},
{_T( ".Net" ), REGEX_COMPATIBILITY_DOTNET, REGEX_SUBSET_DOTNET, NOT_IMPLEMENTED},//Try to include .net implementation for regex
{_T( "PCRE2" ), REGEX_COMPATIBILITY_PCRE2, REGEX_SUBSET_PCRE2, NOT_IMPLEMENTED},//C++ PCRE2
{_T( "Perl" ), REGEX_COMPATIBILITY_PERL, (REGEX_SUBSET_BOOST | REGEX_SUBSET_PERL), TRUE, "Perl", "Perl via Boost Perl (regex_constants::format_perl)", RGB_Cyan},
{_T( "PHP" ), REGEX_COMPATIBILITY_PHP, REGEX_SUBSET_PCRE2, NOT_IMPLEMENTED},//Use with PCRE2, but add conversion for PHP search are replace syntax
{_T( "POSIX_ERE" ), REGEX_COMPATIBILITY_POSIX_ERE, (REGEX_SUBSET_SCINTILLA | REGEX_SUBSET_POSIX), NOT_IMPLEMENTED}, //DO CONVERSIONS ON POSIX CLASS with Scentilla POSIX
{_T( "POSIX_BRE" ), REGEX_COMPATIBILITY_POSIX_BRE, (REGEX_SUBSET_SCINTILLA | REGEX_SUBSET_UNIX_OLD_SYNTAX), TRUE, "", "Old UNIX style regex", RGB_YELLOW},
{_T( "POWERSHELL" ), REGEX_COMPATIBILITY_POWERSHELL, REGEX_SUBSET_DOTNET, NOT_IMPLEMENTED},//Use .net implementation
{_T( "PYTHON" ), REGEX_COMPATIBILITY_PYTHON, REGEX_SUBSET_PYTHON, NOT_IMPLEMENTED},//Try to include python code to do the regex
{_T( "R" ), REGEX_COMPATIBILITY_R, REGEX_SUBSET_PCRE2, NOT_IMPLEMENTED},//Use with PCRE2, but add conversion for R replacement syntax
{_T( "RE2" ), REGEX_COMPATIBILITY_RE2, REGEX_SUBSET_RE2, NOT_IMPLEMENTED},//Use RE2 library
{_T( "RUBY" ), REGEX_COMPATIBILITY_RUBY, (REGEX_SUBSET_BOOST | REGEX_SUBSET_PERL | REGEX_SUBSET_MULTILINE), NOT_IMPLEMENTED},//use boost perl, plus added conversion
{_T( "Scintilla" ), REGEX_COMPATIBILITY_Scintilla, (REGEX_SUBSET_SCINTILLA | REGEX_SUBSET_UNIX_OLD_SYNTAX), TRUE, "Scintilla", "Default Scintilla. Uses Old UNIX regex syntax", RGB_YELLOW},
{_T( "Scintilla POSIX" ), REGEX_COMPATIBILITY_SCINTILLAPOSIX, (REGEX_SUBSET_SCINTILLA | REGEX_SUBSET_POSIX), TRUE, "POSIX", "Scintilla POSIX syntax", RGB_YELLOW},
{_T( "SED" ), REGEX_COMPATIBILITY_SED, (REGEX_SUBSET_BOOST | REGEX_SUBSET_SED), TRUE, "SED", "SED via Boost SED (regex_constants::format_sed)", RGB_Cyan},
{_T( "SED C++" ), REGEX_COMPATIBILITY_CPP_REGEX_SED, (REGEX_SUBSET_STD_REGEX | REGEX_SUBSET_SED), TRUE, "SED", "SED via C++ STD Regex SED (regex_constants::format_sed)", RGB_Orange},
{_T( "TCL" ), REGEX_COMPATIBILITY_TCL, (REGEX_SUBSET_BOOST | REGEX_SUBSET_PERL), TRUE, "TCL", "TCL via Boost Perl (regex_constants::format_perl)", RGB_Cyan},
{_T( "VBSCRIPT" ), REGEX_COMPATIBILITY_VBSCRIPT, (REGEX_SUBSET_BOOST | REGEX_SUBSET_PERL), TRUE, "VBSCRIPT", "Using Boost Perl to emulate VBSCRIPT Regex. (regex_constants::format_perl)", RGB_Cyan},//Same as javascript
{_T( "VIM" ), REGEX_COMPATIBILITY_VIM, (REGEX_SUBSET_BOOST | REGEX_SUBSET_BOOST_ALL | REGEX_SUBSET_MULTILINE), NOT_IMPLEMENTED},//Do conversions with boost all   "Old UNIX Style (Default Scintilla)"
#ifdef _DEBUG //The following are for test purposes only.
{_T( "Testing 1 A Wide String in ComboBox1" ), REGEX_COMPATIBILITY_DUMMY1, (REGEX_SUBSET_BOOST | REGEX_SUBSET_BOOST_ALL | REGEX_SUBSET_MULTILINE), TRUE, "FooFoo1", "", RGB_Lime},
{_T( "Testing 2 A Wide String in ComboBox2" ), REGEX_COMPATIBILITY_DUMMY2, (REGEX_SUBSET_BOOST | REGEX_SUBSET_MULTILINE), TRUE, "FooFoo2", "More Foo Foo2", RGB_YELLOW},
{_T( "Testing 3 A Wide String in ComboBox3" ), REGEX_COMPATIBILITY_DUMMY3, (REGEX_SUBSET_BOOST | REGEX_SUBSET_BOOST_ALL), TRUE, "FooFoo3", "", RGB_Cyan},
{_T( "Testing 4 A Wide String in ComboBox4" ), REGEX_COMPATIBILITY_DUMMY4, (REGEX_SUBSET_BOOST | REGEX_SUBSET_BOOST_ALL | REGEX_SUBSET_MULTILINE), TRUE, "FooFoo4", "More Foo Foo4", RGB_Orange},
{_T( "Testing 5 A Wide String in ComboBox5" ), REGEX_COMPATIBILITY_DUMMY5, (REGEX_SUBSET_SCINTILLA | REGEX_SUBSET_POSIX), TRUE, "FooFoo5", "More Foo Foo5", RGB_Pink}
#endif
};

#define INSERT_ITEMS_LIST( _a, _b, _c, _d )		_T(_b), _T(_a), _T(_c), _T(_d)
const CString CRegexAssistantDlg::InsertItemsList[] =
{
	//Item starting with *, are patterns which can only be used with std::regex or boost regex
	INSERT_ITEMS_LIST( "Any single character", ".", "a.c", "abc, aac, acc, adc, aec, ..." ),
	INSERT_ITEMS_LIST( "0 or more of previous expression", "*", "ab*c", "ac, abc, abbc, abbbc, ..." ),
	INSERT_ITEMS_LIST( "0 or 1 of previous expression", "?", "ab?c", "ac, abc" ),
	INSERT_ITEMS_LIST( "1 or more of previous expression", "+", "ab+c", "abc, abbc, abbbc, ..." ),
	INSERT_ITEMS_LIST( "Start of a string", "^", "^abc", "abc, abcdefg, abc123, ..." ),
	INSERT_ITEMS_LIST( "End of a string", "$", "abc$", "abc, endsinabc, 123abc, ..." ),
	INSERT_ITEMS_LIST( "Choice operator matches either the expression before or after the operator", "|", "abc|def", "abc, def" ),
	INSERT_ITEMS_LIST( "White space", "\\s", "a\\sc", "a c" ),
	INSERT_ITEMS_LIST( "Matches any non-white-space. Same as [^\\s]", "\\S", "axter\\Scom", "axter.com, axter-com" ),
	INSERT_ITEMS_LIST( "Single digit. Same as [0-9]", "\\d", "cat\\dfood", "cat8food, cat9food, cat2food, cat1food" ),
	INSERT_ITEMS_LIST( "Excludes digits. Same as [^0-9]", "\\D", "cat\\Dfood", "cat_food, catifood, cat-food, cat food" ),
	INSERT_ITEMS_LIST( "Any single letter, digit, or underscore. Same as [A-Za-z0-9_]", "\\w", "cat\\wfood", "cat8food, cat_food, catQfood, catifood" ),
	INSERT_ITEMS_LIST( "Exludes letter, digit, or underscore. Same as [^A-Za-z0-9_]", "\\W", "cat\\Wfood", "cat food, cat&food, cat-food, cat.food" ),
	INSERT_ITEMS_LIST( "*Word Boundary", "\\b", "\\babc\\b", "abc" ),
	INSERT_ITEMS_LIST( "Zero or more characters", ".*", "comm.*", "commercial ventures, commerce activity" ),
	INSERT_ITEMS_LIST( "Zero or more character(s), no white space", "\\w*", "comm.\\w*", "commercial, commerce, community" ),
	INSERT_ITEMS_LIST( "(Word) 1 or more character(s), no white space", "\\w+", "\\w+", "abc, defg, foo3, foo_123" ),
	INSERT_ITEMS_LIST( "Any single digit", "[0-9]", "cat[0-9]ood", "cat3ood, cat8ood, cat2ood, ..." ),
	INSERT_ITEMS_LIST( "Any single letter", "[a-zA-Z]", "cat[a-zA-Z]ood", "catfood, catHood, catwood" ),
	INSERT_ITEMS_LIST( "Any single lowercase letter", "[a-z]", "cat[a-z]ood", "catfood, cathood, catwood" ),
	INSERT_ITEMS_LIST( "Any single uppercase letter", "[A-Z]", "cat[A-Z]ood", "catFood, catHood, catWood" ),
	INSERT_ITEMS_LIST( "Any single letter or digit", "[0-9a-zA-Z]", "cat[0-9a-zA-Z]ood", "cat5ood, cathood, catWood" ),
	INSERT_ITEMS_LIST( "Any non-digit", "[^0-9]", "cat[^0-9]ood", "cat@ood, cathood, catWood" ),
	INSERT_ITEMS_LIST( "Any non-letter", "[^a-zA-Z]", "cat[^a-zA-Z]ood", "cat@ood, cat9ood, cat$ood" ),
	INSERT_ITEMS_LIST( "Any printable character or string (non-whitespace) Note: Can be used to exclude empty lines in filter field.", "\\S+", "", "" ),
	INSERT_ITEMS_LIST( "*Lookahead", "(?=)", "foo(?=bar)", "foobar" ),
	INSERT_ITEMS_LIST( "*Lookahead Not", "(?!)", "foo(?!bar)", "food foofoo" ),
	//ToDo: Check if Lookbehind is supported by the BOOST
	//"*Lookbehind", "", "(?<=bar)foo", ""),
	//"*Lookbehind Not", "", "(?<!bar)foo", ""),
	INSERT_ITEMS_LIST( "Windows return/new line", "\\r\\n", "", "" ),
	INSERT_ITEMS_LIST( "UNIX new line", "\\n", "", "" ),
	INSERT_ITEMS_LIST( "(Win-Local) File name pattern", "[a-zA-Z]:\\\\[0-9a-zA-Z\\\\\\s\\-_]+\\.[0-9a-zA-Z_]+", "", "C:\\program\\filename.txt, d:\\foofoo.doc" ),
	INSERT_ITEMS_LIST( "(Win-UNC) File name pattern", "\\\\\\\\[0-9a-zA-Z\\\\\\s\\-_]+\\.[0-9a-zA-Z_]+", "", "\\\\axter\\devl\\dmaisonave\\filename.txt, \\\\DevMain\\MyShare\\foofoo.doc" ),
	INSERT_ITEMS_LIST( "(UNIX) File name pattern", "/[0-9a-zA-Z/\\s\\-_]+\\.[0-9a-zA-Z_]+", "", "/var/log/data.txt" ),
	INSERT_ITEMS_LIST( "Email pattern", "[0-9a-zA-Z\\-\\s_]+@[0-9a-zA-Z\\-_]+\\.[0-9a-zA-Z]+", "", "foofoo@axter.com, bill@foofoo.com, ..." ),
	INSERT_ITEMS_LIST( "GUID pattern", "[0-9a-fA-F]+-[0-9a-fA-F]+-[0-9a-fA-F]+-[0-9a-fA-F]+-[0-9a-fA-F]+", "", "E6710055-F6FF-4f60-BF39-EF3372B2A48B" ),
	INSERT_ITEMS_LIST( "*Error reporting pattern", "(?i)fail|error|warn|exception|err\\s?no", "", "Error, Warn, Warning, Exception, exception, FAIL" ),
	INSERT_ITEMS_LIST( "*yyyy-mm-dd date format", "^(19|20)\\d\\d[- /.](0[1-9]|1[012])[- /.](0[1-9]|[12][0-9]|3[01])", "", " 2012-01-31,  1999-12-11,  2008/1/31" ),
	INSERT_ITEMS_LIST( "*dd-mm-yyyy date format", "^(0[1-9]|1[012])[- /.](0[1-9]|[12][0-9]|3[01])[- /.](19|20)\\d\\d", "", "07-03-1961, 21/11/1984, 31-1-2000" ),
	INSERT_ITEMS_LIST( "*dd-mm date format", "^(0[1-9]|1[012])[- /.](0[1-9]|[12][0-9]|3[01])", "", "07-03, 21/11, 31-1" ),
	INSERT_ITEMS_LIST( "*mmm dd, yyyy", "[A-Z][a-z][a-z] [0-9][0-9]*, [0-9]{4}", "", "Jan 3, 2003" ),
	INSERT_ITEMS_LIST( "*Time patterns", "((0?[1-9]|1[012])(\\s*:\\s*([0-5]\\d))?(\\s*:\\s*([0-5]\\d))?(\\s*([AaPp])[Mm]?)$|(2[0-3]|[1]\\d|0?\\d)(\\s*:\\s*([0-5]\\d))(\\s*:\\s*([0-5]\\d))?)", "", "3:00:59 | 12pm | 6:30 a" ),
	INSERT_ITEMS_LIST( "*US phone no. pattern [word]", "\\b([0-9]{3})-([0-9]{3})-([0-9]{4})\\b", "", "111-123-4567" ),
	INSERT_ITEMS_LIST( "*US phone no. pattern", "((\\(\\d{3}\\))|(\\d{3}-))\\d{3}-\\d{4}", "", "(111)123-4567, 111-123-4567, (111)-123-4567" ),
	INSERT_ITEMS_LIST( "*International & Domestic Phone Numbers with Ext", "([\\+][0-9]{1,3}([ \\.\\-])?)?([\\(]{1}[0-9]{3}[\\)])?([0-9A-Z \\.\\-]{1,32})((x|ext|extension)?[0-9]{1,4}?)", "", "(123)456-7890 | (123)456-7890 x123 | +1 (123)456-7890 | 12 3456 789 0 x1234 | (123)456-7890x123 |(123)456-7890ext123 | (123)456-7890 extension123 | 123.456.7890 | 1234567890 | 1234567 | 12 34 56 78 90 | 12 3 4567 890123 x4567 | +12 3456 7890 | +12 34 56 7890 | +12 3456 7890 | +12 34567890" ),
	INSERT_ITEMS_LIST( "*IP pattern", "\\b(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\b", "", "192.168.0.3, 255.255.255.255" ),
	INSERT_ITEMS_LIST( "*Social security number pattern", "[0-9]{3}-[0-9]{2}-[0-9]{4}", "", "" ),
	INSERT_ITEMS_LIST( "*Float pattern", "[-+]?([0-9]+)?\\.[0-9]+", "", "123.456, 0.99, .987" ),
	INSERT_ITEMS_LIST( "*Zip code pattern", "[0-9]{5}(-[0-9]{4})?", "", "19133, 08015-1234" ),
	INSERT_ITEMS_LIST( "*URL pattern", "http(s)*://(([\\-\\w]+\\.)+[a-zA-Z]{2,4}.*)", "", "http://axter.com, http://axter.com, https://axter.com" ),
	INSERT_ITEMS_LIST( "*FTP pattern", "ftp://([\\w](:[\\w]))*(([\\-\\w]+\\.)+[a-zA-Z]{2,4}[/\\w]*)", "", "" ),
	INSERT_ITEMS_LIST( "*Domain pattern", "[\\-\\w]+\\.)+[a-zA-Z]{2,4}", "", "" ),
	INSERT_ITEMS_LIST( "*Major credit card pattern", "\\d{4}-\\d{4}-\\d{4}-\\d{4}", "", "" ),
	INSERT_ITEMS_LIST( "*User name pattern", "[\\s\\[][a-zA-Z0-9_-]{3,16}\\\\[a-z0-9_-]{3,16}[\\s\\]]", "", "axter-nj\\david" ),
	INSERT_ITEMS_LIST( "HTML tag pattern", "</?[a-zA-Z]+/?>", "", "" ),
	INSERT_ITEMS_LIST( "*Web color pattern", "#?(([fFcC0369])\\2){3}", "", "#FFFFFF | FFCC00 | 003300" ),
	INSERT_ITEMS_LIST( "*SQL basic query patterns", "(?i)(SELECT\\s[\\w\\*\\)\\(\\,\\s]+\\sFROM\\s[\\w]+)|(?i)(UPDATE\\s[\\w]+\\sSET\\s[\\w\\,\\'\\=]+)|(?i)(INSERT\\sINTO\\s[\\d\\w]+[\\s\\w\\d\\)\\(\\,]*\\sVALUES\\s\\([\\d\\w\\'\\,\\)]+)|(?i)(DELETE\\sFROM\\s[\\d\\w\\'\\=]+)", "", "SELECT * FROM TABLE, insert into table, UPDATE TABLE SET FIELD=VALUE WHERE ID_FIELD=VALUE_ID, DELETE FROM TABLE WHERE" ),
	INSERT_ITEMS_LIST( "*Collect file pattern", "([a-zA-Z]:(\\\\w+)*\\\\[a-zA-Z0_9]+)?.cvf", "", "c:\\Program\\Data\\CollectInc.cvf" ),
	INSERT_ITEMS_LIST( "Find function with 1 argument pattern", "(func_name)\\s*\\(\\s*([0-9a-zA-Z_\"']*)\\s*\\)", "(CreateFoo)\\s*\\(\\s*([0-9a-zA-Z_\"']*)\\s*\\)", "CreateFoo(ArgData)" ),
	INSERT_ITEMS_LIST( "Find function with 2 arguments pattern", "(func_name)\\s*\\(\\s*([0-9a-zA-Z_\"']*)\\s*\\,\\s*([0-9a-zA-Z_\"']*)\\s*\\)", "(CreateFoo)\\s*\\(\\s*([0-9a-zA-Z_\"']*)\\s*\\,\\s*([0-9a-zA-Z_\"']*)\\s*\\)", "CreateFoo(ArgData, arg2)" ),
	INSERT_ITEMS_LIST( "Find function with 3 arguments pattern", "(func_name)\\s*\\(\\s*([0-9a-zA-Z_\"']*)\\s*\\,\\s*([0-9a-zA-Z_\"']*)\\s*\\,\\s*([0-9a-zA-Z_\"']*)\\s*\\)", "(CreateFoo)\\s*\\(\\s*([0-9a-zA-Z_\"']*)\\s*\\,\\s*([0-9a-zA-Z_\"']*)\\s*\\,\\s*([0-9a-zA-Z_\"']*)\\s*\\)", "CreateFoo(ArgData, arg2, arg3)" ),
	INSERT_ITEMS_LIST( "Find function with 4 arguments pattern", "(func_name)\\s*\\(\\s*([0-9a-zA-Z_\"']*)\\s*\\,\\s*([0-9a-zA-Z_\"']*)\\s*\\,\\s*([0-9a-zA-Z_\"']*)\\s*\\,\\s*([0-9a-zA-Z_\"']*)\\s*\\)", "(CreateFoo)\\s*\\(\\s*([0-9a-zA-Z_\"']*)\\s*\\,\\s*([0-9a-zA-Z_\"']*)\\s*\\,\\s*([0-9a-zA-Z_\"']*)\\s*\\,\\s*([0-9a-zA-Z_\"']*)\\s*\\)", "CreateFoo(ArgData, arg2, arg3, arg4)" ),
	INSERT_ITEMS_LIST( "Find function with 5 arguments pattern", "(func_name)\\s*\\(\\s*([0-9a-zA-Z_\"']*)\\s*\\,\\s*([0-9a-zA-Z_\"']*)\\s*\\,\\s*([0-9a-zA-Z_\"']*)\\s*\\,\\s*([0-9a-zA-Z_\"']*)\\s*\\,\\s*([0-9a-zA-Z_\"']*)\\s*\\)", "(CreateFoo)\\s*\\(\\s*([0-9a-zA-Z_\"']*)\\s*\\,\\s*([0-9a-zA-Z_\"']*)\\s*\\,\\s*([0-9a-zA-Z_\"']*)\\s*\\,\\s*([0-9a-zA-Z_\"']*)\\s*\\,\\s*([0-9a-zA-Z_\"']*)\\s*\\)", "CreateFoo(ArgData, arg2, arg3, arg4, arg5)" ),
	//Item starting with ~, (\1\2) works with Boost Regex, Scintilla-Standard, POSIX, boost Perl, Boost Sed, STD SED
	//Item starting with $, ($1$2) works with Boost Regex, boost Perl, Boost Multiline, boost default, STD regex
	//Item starting with 1,2,or 3 in the second character of the second column, are patterns which can only be used for replacement regex field, and when double clicked will get inserted into replacement field.
	INSERT_ITEMS_LIST( "~1st Argument for replacement token", "\\1", "", "" ),
	INSERT_ITEMS_LIST( "~2nd Argument for replacement token", "\\2", "", "" ),
	INSERT_ITEMS_LIST( "~3rd Argument for replacement token", "\\3", "", "" ),
	INSERT_ITEMS_LIST( "$1st Argument for replacement token", "$1", "", "" ),
	INSERT_ITEMS_LIST( "$2nd Argument for replacement token", "$2", "", "" ),
	INSERT_ITEMS_LIST( "$3rd Argument for replacement token", "$3", "", "" ),
	INSERT_ITEMS_LIST( "$Alternate syntax for replacement token", "$1($2)", "", "" ),
	INSERT_ITEMS_LIST( "", "", "", "" )
	/*  Note: Scintilla standard uses following systax \(foo\)\(\S*\) and replacement boo\2 vs Scintilla POSIX uses syntax (foo)(\S*)
		Test out the following VIM tokens, and if they down't work, see if they can get converted to Perl syntax
			\_s 				[ \t\r\n\v\f] or \s
			\u					[A-Z]
			\l					[a-z]
			\x					[A-Fa-f0-9]
			\p					[\x20-\x7E]
			\a					[A-Za-z]

		Check if the following java tokens can be converted to Perl syntax
			\p{ASCII}			[\x00-\x7F]
			\p{Alnum} 			[A-Za-z0-9]
			\p{Alpha}			[A-Za-z]
			\p{Blank} 			[ \t]
			\p{Cntrl} 			[\x00-\x1F\x7F]
			\p{Graph} 			[\x21-\x7E]
			\p{Lower} 			[a-z]
			\p{Print} 			[\x20-\x7E]
			\p{Punct} 			[][!"#$%&'()*+,./:;<=>?@\^_`{|}~-]
			\p{Upper} 			[A-Z]
			\p{XDigit} 			[A-Fa-f0-9]
			\p{Digit} or \d 	[0-9]
			\p{Space} or \s 	[ \t\r\n\v\f]

		Check if the following POSIX tokens work or can be converted to Perl syntax
			[:alnum:]			[A-Za-z0-9] 	Alphanumeric characters
			[:alpha:]			[A-Za-z] 	Alphabetic characters
			[:blank:]			[ \t] 	Space and tab
			[:cntrl:]			[\x00-\x1F\x7F] 	Control characters
			[:digit:]			[0-9] 	Digits
			[:graph:]			[\x21-\x7E] 	Visible characters
			[:lower:]			[a-z] 	Lowercase letters
			[:print:]			[\x20-\x7E] 	Visible characters and the space character
			[:punct:]			[][!"#$%&'()*+,./:;<=>?@\^_`{|}~-] 	Punctuation characters
			[:space:]			[ \t\r\n\v\f] 	Whitespace characters
			[:upper:]			[A-Z] 	Uppercase letters
			[:xdigit:]			[A-Fa-f0-9] 	Hexadecimal digits

		*Non-standard
			[:ascii:]			[\x00-\x7F] 	ASCII characters
			[:word:]			[A-Za-z0-9_] 	Alphanumeric characters plus "_"
	*/
};
const int CRegexAssistantDlg::QtyColumnsInLinst = 4;
const int CRegexAssistantDlg::MaxInsertItemsList = ((sizeof( InsertItemsList ) / sizeof( InsertItemsList[0] ))) / QtyColumnsInLinst;
const CString CRegexAssistantDlg::m_DefaultTestTargetTextData = _T( "\
Paste your sample text here. The text here becomes highlighted when it matches Regex code (Expression Field). By default this program starts up with the [Sample] and [Expression] fields populated, to help the user quickly learn by example.\r\n\
Here is an exercise to get more familiar with the program interface.\r\n\
1. Delete Expression field, and leave the default content in the sample field.\r\n\
2. Type foo		While typing, notice what is getting highlighted in the sample field.\r\n\
3. Leave the cursor at the end, and double click \\S on the token list. Make sure to double click the upper case \\S.  Notice an additional single charcter get highlighted.\r\n\
4. Reposition the cursor to the end, and double click * on the token list. Notice the remaining characters in the words get highlighted.\r\n\
5. Reposition the cursor to the end, and double click $ on the token list. Notice only the matching words that are at the end of a line are highlighted.\r\n\
6. Reposition the cursor to the start, and double click the ^ on the token list. Notice only matching single word lines are highlighted.\r\n\
foolhardier\r\n\
foolery frog beach cake foothill run fish foul ball film star flight footlights space footstools\r\n\
Footnotes said the footdraggers took footage of a foolish footman as he foozled the putt playing golf in a football field\r\n\
David posted file c:\\foothill.doc to site http://www.foodie.com and put a copy in the footlocker\r\n\
David Maisonave plays football better then Foosball, but he is footloose in zip code 08015-6912\r\n\
The IP address 10.165.132.95 comes from a 19133 zip code area, which is where 800-969-2030 and 555.345.1200 is located.\r\n\
c:\\film.txt          foozle\r\n\
footlights           David\r\n\
foodie               Friday\r\n\
More test text below......\r\n\
/sub/subsub/subsubsub/footsteps/footlocker.cvf\r\n\
732-123-4567         http://foolhardy.com\r\n\
http://axter.com     732-987-6543\r\n\
800-888-8933         ftp://foozled.com\r\n\
1-800-325-9345       http://footloose.net\r\n\
10.0.0.123           134-42-0156\r\n\
459-78-2390          10.0.0.101 \r\n\
" );