//Note: This file should only be included in RegexAssistantDlg.cpp
//Initialize static members of CRegexAssistantDlg
#include "RegexAssistantDlg_enums.h"

const MARKERDATAtag CRegexAssistantDlg::m_MarkerData[] = {
{RGB( 0, 0, 0 )/*Black*/, RGB( 0, 255, 0 )/*Lime*/, FALSE},       //(Green) For Boost
{RGB( 0, 0, 0 )/*Black*/, RGB( 255, 255, 0 )/*Yellow*/, FALSE},   // For REGEX_COMPATIBILITY_SCINTILLA_POSIX
{RGB( 0, 0, 0 )/*Black*/, RGB( 0, 255, 255 )/*Cyan*/, FALSE},     //For STD Regex and STD SED
{RGB( 0, 0, 0 /*Black*/ ), RGB( 255, 128, 0 )/*Orange*/, FALSE},  //For REGEX_COMPATIBILITY_SCINTILLA
{RGB( 0, 0, 0 /*Black*/ ), RGB( 254, 185, 170 )/*Pink*/, FALSE}  //For Java
};



#define NOT_IMPLEMENTED				FALSE, "", "Awaiting Implementation", 0

const RegexCompatibilityProperties CRegexAssistantDlg::m_RegexCompatibilityProperties[] =
{
{_T( "POSIX" ), REGEX_COMPATIBILITY_SCINTILLA_POSIX, (REGEX_SUBSET_SCINTILLA | REGEX_SUBSET_POSIX | REGEX_SUBSET_BACKSLASH_PREFIX_ONLY), TRUE, "POSIX", "Scintilla POSIX syntax", RGB_YELLOW},
{_T( "All" ), REGEX_COMPATIBILITY_ALL, (REGEX_SUBSET_BOOST | REGEX_SUBSET_BOOST_ALL | REGEX_SUBSET_MULTILINE), NOT_IMPLEMENTED}, //Works with most regex syntax. In the backend, some syntax gets converted to work with Boost All
{_T( "Boost - All" ), REGEX_COMPATIBILITY_BOOST_ALL_STANDARD, (REGEX_SUBSET_BOOST | REGEX_SUBSET_BOOST_ALL | REGEX_SUBSET_MULTILINE), TRUE, "BoostAll", "Boost with flag regex_constants::format_all", RGB_Cyan},
{_T( "Boost ECMAScript (default)" ), REGEX_COMPATIBILITY_BOOST, REGEX_SUBSET_BOOST | REGEX_SUBSET_DOLLAR_PREFIX_ONLY | REGEX_SUBSET_MULTILINE, TRUE, "Boost", "Default boost regex with flag regex_constants::format_default", RGB_Orange},
{_T( "Boost Perl" ), REGEX_COMPATIBILITY_BOOST_PERL_MULTILINE, (REGEX_SUBSET_BOOST | REGEX_SUBSET_PERL | REGEX_SUBSET_MULTILINE), TRUE, "BoostPerl", "Boost with Perl (regex_constants::format_perl) and Multiline set", RGB_Cyan},
{_T( "Boost SED" ), REGEX_COMPATIBILITY_SED, (REGEX_SUBSET_BOOST | REGEX_SUBSET_SED | REGEX_SUBSET_BACKSLASH_PREFIX_ONLY | REGEX_SUBSET_MULTILINE), TRUE, "SED", "SED via Boost SED (regex_constants::format_sed)", RGB_YELLOW},
{_T( "C++ STD Regex ECMAScript" ), REGEX_COMPATIBILITY_ECMAScript, REGEX_SUBSET_STD_REGEX | REGEX_SUBSET_DOLLAR_PREFIX_ONLY | REGEX_SUBSET_MULTILINE, TRUE, "ECMAScript", "ECMAScript via standard default STD C++ Regex", RGB_Orange},
{_T( "C++ STD Regex Extended" ), REGEX_COMPATIBILITY_CPP_EXT, (REGEX_SUBSET_STD_REGEX | REGEX_SUBSET_DOLLAR_PREFIX_ONLY | REGEX_SUBSET_STD_REGEX_EXT | REGEX_SUBSET_MULTILINE), TRUE, "CXX", "C++ STD Regex Extended", RGB_Orange},
{_T( "C++ STD Regex Basic" ), REGEX_COMPATIBILITY_CPP_BASIC, (REGEX_SUBSET_STD_REGEX | REGEX_SUBSET_DOLLAR_PREFIX_ONLY | REGEX_SUBSET_STD_REGEX_BASIC | REGEX_SUBSET_MULTILINE), TRUE, "C++", "C++ STD Regex Basic", RGB_Orange},
{_T( "C++ STD Regex SED" ), REGEX_COMPATIBILITY_CPP_REGEX_SED, (REGEX_SUBSET_STD_REGEX | REGEX_SUBSET_SED | REGEX_SUBSET_BACKSLASH_PREFIX_ONLY | REGEX_SUBSET_MULTILINE), TRUE, "CPPSED", "SED via C++ STD Regex SED (regex_constants::format_sed)", RGB_YELLOW},
{_T( "Scintilla" ), REGEX_COMPATIBILITY_Scintilla, (REGEX_SUBSET_SCINTILLA | REGEX_SUBSET_UNIX_OLD_SYNTAX | REGEX_SUBSET_BACKSLASH_PREFIX_ONLY), TRUE, "Scintilla", "Default Scintilla. Uses Old UNIX regex syntax", RGB_YELLOW},
{_T( "Scintilla POSIX" ), REGEX_COMPATIBILITY_SCINTILLAPOSIX, (REGEX_SUBSET_SCINTILLA | REGEX_SUBSET_POSIX | REGEX_SUBSET_BACKSLASH_PREFIX_ONLY), TRUE, "POSIX", "Scintilla POSIX syntax", RGB_YELLOW},
{_T( "Scintilla SED" ), REGEX_COMPATIBILITY_SCINTILLAPOSIX, (REGEX_SUBSET_SCINTILLA | REGEX_SUBSET_SED | REGEX_SUBSET_BACKSLASH_PREFIX_ONLY), TRUE, "ScintillaSED", "Scintilla with flag CXX11REGEX, which uses the C++ Regex (SED) built with Scintilla. **Warning: Replacement logic has very poor performance", RGB_YELLOW},
{_T( "Groovy" ), REGEX_COMPATIBILITY_GROOVY, REGEX_SUBSET_JAVA, NOT_IMPLEMENTED},// Same as Java
{_T( "Java" ), REGEX_COMPATIBILITY_JAVA, REGEX_SUBSET_JAVA, NOT_IMPLEMENTED},//Try implementing java code in this application, or do conversions with boost all
{_T( "JavaScript" ), REGEX_COMPATIBILITY_JAVASCRIPT, (REGEX_SUBSET_BOOST | REGEX_SUBSET_PERL), TRUE, "JavaScript", "JavaScript uses syntax similar to Perl.  Using Boost Perl (regex_constants::format_perl) to emulate JavaScript Regex.", RGB_Cyan},
{_T( ".Net" ), REGEX_COMPATIBILITY_DOTNET, REGEX_SUBSET_DOTNET, NOT_IMPLEMENTED},//Try to include .net implementation for regex
{_T( "PCRE2" ), REGEX_COMPATIBILITY_PCRE2, REGEX_SUBSET_PCRE2, NOT_IMPLEMENTED},//C++ PCRE2
{_T( "PHP" ), REGEX_COMPATIBILITY_PHP, REGEX_SUBSET_PCRE2, NOT_IMPLEMENTED},//Use with PCRE2, but add conversion for PHP search are replace syntax
{_T( "POSIX-ERE" ), REGEX_COMPATIBILITY_POSIX_ERE, (REGEX_SUBSET_SCINTILLA | REGEX_SUBSET_POSIX | REGEX_SUBSET_BACKSLASH_PREFIX_ONLY), NOT_IMPLEMENTED}, //DO CONVERSIONS ON POSIX CLASS with Scentilla POSIX
{_T( "POSIX-BRE" ), REGEX_COMPATIBILITY_POSIX_BRE, (REGEX_SUBSET_SCINTILLA | REGEX_SUBSET_UNIX_OLD_SYNTAX | REGEX_SUBSET_BACKSLASH_PREFIX_ONLY), TRUE, "", "Old UNIX style regex", RGB_YELLOW},
{_T( "POWERSHELL" ), REGEX_COMPATIBILITY_POWERSHELL, REGEX_SUBSET_DOTNET, NOT_IMPLEMENTED},//Use .net implementation
{_T( "PYTHON" ), REGEX_COMPATIBILITY_PYTHON, REGEX_SUBSET_PYTHON, NOT_IMPLEMENTED},//Try to include python code to do the regex
{_T( "R" ), REGEX_COMPATIBILITY_R, REGEX_SUBSET_PCRE2, NOT_IMPLEMENTED},//Use with PCRE2, but add conversion for R replacement syntax
{_T( "RE2" ), REGEX_COMPATIBILITY_RE2, REGEX_SUBSET_RE2, NOT_IMPLEMENTED},//Use RE2 library
{_T( "RUBY" ), REGEX_COMPATIBILITY_RUBY, (REGEX_SUBSET_BOOST | REGEX_SUBSET_PERL | REGEX_SUBSET_MULTILINE), NOT_IMPLEMENTED},//use boost perl, plus added conversion
{_T( "TCL" ), REGEX_COMPATIBILITY_TCL, (REGEX_SUBSET_BOOST | REGEX_SUBSET_PERL), TRUE, "TCL", "TCL via Boost Perl (regex_constants::format_perl)", RGB_Cyan},
{_T( "VBSCRIPT" ), REGEX_COMPATIBILITY_VBSCRIPT, (REGEX_SUBSET_BOOST | REGEX_SUBSET_PERL), TRUE, "VBSCRIPT", "Using Boost Perl to emulate VBSCRIPT Regex. (regex_constants::format_perl)", RGB_Cyan},//Same as javascript
{_T( "VIM" ), REGEX_COMPATIBILITY_VIM, (REGEX_SUBSET_BOOST | REGEX_SUBSET_BOOST_ALL | REGEX_SUBSET_MULTILINE), NOT_IMPLEMENTED},//Do conversions with boost all   "Old UNIX Style (Default Scintilla)"
//#ifdef _DEBUG //The following are for test purposes only.
//{_T( "Testing 1 A Wide String in ComboBox1" ), REGEX_COMPATIBILITY_DUMMY1, (REGEX_SUBSET_BOOST | REGEX_SUBSET_BOOST_ALL | REGEX_SUBSET_MULTILINE), TRUE, "FooFoo1", "", RGB_Lime},
//{_T( "Testing 2 A Wide String in ComboBox2" ), REGEX_COMPATIBILITY_DUMMY2, (REGEX_SUBSET_BOOST | REGEX_SUBSET_MULTILINE), TRUE, "FooFoo2", "More Foo Foo2", RGB_YELLOW},
//{_T( "Testing 3 A Wide String in ComboBox3" ), REGEX_COMPATIBILITY_DUMMY3, (REGEX_SUBSET_BOOST | REGEX_SUBSET_BOOST_ALL), TRUE, "FooFoo3", "", RGB_Cyan},
//{_T( "Testing 4 A Wide String in ComboBox4" ), REGEX_COMPATIBILITY_DUMMY4, (REGEX_SUBSET_BOOST | REGEX_SUBSET_BOOST_ALL | REGEX_SUBSET_MULTILINE), TRUE, "FooFoo4", "More Foo Foo4", RGB_Orange},
//{_T( "Testing 5 A Wide String in ComboBox5" ), REGEX_COMPATIBILITY_DUMMY5, (REGEX_SUBSET_SCINTILLA | REGEX_SUBSET_POSIX | REGEX_SUBSET_BACKSLASH_PREFIX_ONLY), TRUE, "FooFoo5", "More Foo Foo5", RGB_Pink}
//#endif
};

#define INSERT_ITEMS_LIST( _a, _b, _c, _d, _e )		_T(_b), _T(_a), _T(_c), _T(_e), _T(_d)
const CString CRegexAssistantDlg::InsertItemsList[] =
{
	//ToDo: Need to add \G \A \Z \z \X \C \R \N \v \V \h \H \K \# \pX \p{...} \PX \Q \xYY \x{YYYY} \ddd \cY [\b] \U\1\E\2 \L\1\E\2
	//                  (\f matches form-feed)
	//Item starting with *, are patterns which does not work on Scintilla default and Scintilla POSIX
	//Item starting with \, (\1\2) works with Boost Regex, *Scintilla-Standard, *POSIX, boost Perl, *Boost Sed, *STD SED
	//Item starting with $, ($1$2) works with Boost Regex, boost Perl, *boost default, *STD regex
	//Item starting with @, are patterns which works with boost
	//Item starting with <, are patterns which does not work with SED and C++
	INSERT_ITEMS_LIST( "A single character",																	".",			"a.c", "abc, aac, acc, adc, aec", "abc cba ABC aacc aaaa adcc aece" ),
	INSERT_ITEMS_LIST( "Zero or more of previous expression",													"*",			"ab*c", "abc, ac, abbbbc", "abc ABC cba aacc aabb aabbbbc" ),
	INSERT_ITEMS_LIST( "Zero or 1 of previous expression",														"?",			"ab?c", "abc, ac", "abc ABC cba aacc aabb aabbbbc" ),
	INSERT_ITEMS_LIST( "1 or more of previous expression",														"+",			"ab+c", "abc, abbbbc", "abc ABC cba aacc aabb aabbbbc" ),
	INSERT_ITEMS_LIST( "*Matches either the expression before or after",										"|",			"abc|def", "abc, def, def, abcdef, abc, def", "abc ABC default bcdef abcdefghi 123abc define" ),
	INSERT_ITEMS_LIST( "Beginning of a line",																	"^",			"^foo\\S*", "*See sample text" , "foolhardier,foolery, footlights, foodie"),
	INSERT_ITEMS_LIST( "End of a line",																			"$",			"\\S*com$", "*See sample text" , "http://foolhardy.com, ftp://foozled.com"),
	INSERT_ITEMS_LIST( "<Beginning of a word",																	"\\<",			"\\<foot\\S*", "foothill, footstools", "foothill bigfoot footstools Footage" ),
	INSERT_ITEMS_LIST( "<End of a word",																		"\\>",			"\\S*ch\\>", "beach, which, crunch", "beach chair which crunches crunch" ),
	INSERT_ITEMS_LIST( "White space",																			"\\s",			"fo[a-z]*\\sball", "foul ball, foot ball", "foul ball foulball football foot ball" ),
	INSERT_ITEMS_LIST( "Matches any non-white-space. Same as [^\\s]",											"\\S",			"\\Saxter\\Scom", "See sample text.", "http://axter.com, http://www.axter.com, ftp://davidjr.axter.com,  maisonave-axter-com" ),
	INSERT_ITEMS_LIST( "Single digit. Same as [0-9]",															"\\d",			"cat\\df\\S*", "cat8fish, cat9food, cat2food", "cat8fish cat fish cat9food cat11food cat2food cat food" ),
	INSERT_ITEMS_LIST( "Excludes digits. Same as [^0-9]",														"\\D",			"cat\\Df\\S*", "cat fish, cat-fall, cat_food, catyfoo", "cat8fish cat fish cat9food cat-fall cat2food cat_food catfood catyfoo" ),
	INSERT_ITEMS_LIST( "A letter, digit, or underscore. Same as [A-Za-z0-9_]",									"\\w",			"cat\\wf\\S*", "cat8food, cat9food, catAfood, cat_food, catyfoo", "cat8fish cat fish catAfood cat-fall cat2food cat_food catfood catyfoo" ),
	INSERT_ITEMS_LIST( "Exludes letters, digits, & underscore. Same as [^A-Za-z0-9_]",							"\\W",			"cat\\Wf\\S*", "cat fish, cat-fall, cat.found, cat&food", "cat8fish cat fish catAfood cat-fall cat.found cat_food catfood cat&food" ),
	INSERT_ITEMS_LIST( "*Word Boundary (word starting with)",													"\\b",			"\\bball", "See sample text.", "See sample text." ),
	INSERT_ITEMS_LIST( "*Word Boundary (word end with",															"\\b",			"ball\\b", "See sample text.", "See sample text." ),
	INSERT_ITEMS_LIST( "*Non-word Boundary (does not start with)",												"\\B",			"\\Bca", "See sample text.", "See sample text." ),
	INSERT_ITEMS_LIST( "*Non-word Boundary (does not end with",													"\\B",			"ca\\B", "See sample text.", "See sample text." ),
	INSERT_ITEMS_LIST( "Zero or more character(s), no white space",												"\\S*",			"foot\\S*", "foot, foothill, foot, football", "Fool foot foulball foothill ballfoot football Footnotes" ),
	INSERT_ITEMS_LIST( "1 or more character(s), no white space",												"\\S+",			"foot\\S+", "foot ball, foothill, football", "Fool foot foot-ball foothill ballfoot football Footnotes" ),
	INSERT_ITEMS_LIST( "Zero or more letters, digits, or underscores",											"\\w*",			"foot\\w*", "foot, foot, foothill, foot, football", "Fool foot foot-ball foothill ballfoot football Footnotes" ),
	INSERT_ITEMS_LIST( "1 or more character(s), no white space",												"\\w+",			"foot\\w+", "foothill, football", "Fool foot foot-ball foothill ballfoot football Footnotes" ),
	INSERT_ITEMS_LIST( "0 or more; NOT Recommended!!!; Recommend using \\S*, \\w* instead",						".*",			"^f.*", "foolhardier", "This pattern commonly produces unwanted results. Works similar to $. Use \\S*, \\w*, \\S+, or \\w++ instead ;*See sample text." ),
	INSERT_ITEMS_LIST( "Single digit. Same as \\d",																"[0-9]",		"cat[0-9]f\\S*", "cat8fish, cat9food, cat2food", "cat8fish cat fish cat9food cat11food cat2food cat food" ),
	INSERT_ITEMS_LIST( "Single letter",																			"[a-zA-Z]",		"cat[a-zA-Z]f\\S*", "catAfood, catzfood, catifoo", "catAfood cat fish catzfood cat-fall cat2food cat_food catfood catifoo" ),
	INSERT_ITEMS_LIST( "Single lowercase letter",																"[a-z]",		"cat[a-z]f\\S*", "catzfood, catifoo", "catAfood cat fish catzfood cat-fall cat2food cat_food catfood catifoo" ),
	INSERT_ITEMS_LIST( "Single uppercase letter",																"[A-Z]",		"cat[A-Z]f\\S*", "catAfood", "catAfood cat fish catzfood cat-fall cat2food cat_food catfood catifoo" ),
	INSERT_ITEMS_LIST( "Single letter or digit",																"[0-9a-zA-Z]",	"cat[0-9a-zA-Z]f\\S*", "catAfood, catzfood, cat2food, catifoo", "catAfood cat fish catzfood cat-fall cat2food cat_food catfood catifoo" ),
	INSERT_ITEMS_LIST( "Non-digit. Same as \\D",																"[^0-9]",		"cat[^0-9]f\\S*", "cat fish, cat-fall, cat_food, catyfoo", "cat8fish cat fish cat9food cat-fall cat2food cat_food catfood catyfoo" ),
	INSERT_ITEMS_LIST( "Non-letter",																			"[^a-zA-Z]",	"cat[^a-zA-Z]f\\S*", "cat fish, cat-fall, cat2food, cat_food", "cat8fish cat fish cat9food cat-fall cat2food cat_food catfood catyfoo" ),
	INSERT_ITEMS_LIST( "@Start case-insensitive match",															"(?i)",			"(?i)foo\\S*", "foothill, FOOTHILL, Foothill, FootHill", "foothill, FOOTHILL, foFootball, Football" ),
	INSERT_ITEMS_LIST( "@Start (default) case-sensitive match",													"(?-i)",		"(?i)foo\\S*", "foothill", "foothill, FOOTHILL, Football, Football" ),
	INSERT_ITEMS_LIST( "*Lookahead",																			"(?=)",			"fo(?=ot)\\S*", "foot, football, foottoe, foothills", "fool foot foosball football barefoottoe foolish highfoothills Footnotes" ),
	INSERT_ITEMS_LIST( "*Negative Lookahead",																	"(?!)",			"fo(?!ot)\\S*", "fool, foosball, foolish", "fool foot foosball football barefoottoe foolish highfoothills Footnotes" ),
	INSERT_ITEMS_LIST( "@Lookbehind",																			"(?<=)",		"(?<=fo\\w\\w)ball\\S*", "ball, ballSeason, ballScores, balls, ballPlayers, ballplayers", "foosball footballSeason Football bigfoot footballScores foulballs footballPlayers footballplayers" ),
	INSERT_ITEMS_LIST( "@Negative Lookbehind",																	"(?<!)",		"(?<!foot)ball\\S*", "balltable, ballPlayers, balls", "foosballtable football FootballPlayers footballScores foulballs" ),
	INSERT_ITEMS_LIST( "*Matches either expression. This or that (?:this|that)",								"(?:)",			"(?:foos|foot|base)ball\\S*", "foosball, football, baseballs, footballScores", "foosball football Football baseballs bigfoot footballScores foulball" ),
	INSERT_ITEMS_LIST( "*Quantity of matches",																	"{#}",			"o{2}\\S", "oot, oos, ool, ood, ooz", "football fod Foosball toast tool told foodie foozled" ),
	INSERT_ITEMS_LIST( "*Match quantity X or more",																"{X,}",			"o{1,}\\S", "oot, od, oos, oa, ool, ol, ood, ooz", "football fod Foosball toast tool told foodie foozled" ),
	INSERT_ITEMS_LIST( "*Quantity of matches from one number to the other",										"{#,#}",		"o{1,2}\\S", "oot, od, oos, oa, ool, ol, ood, ooz", "football fod Foosball toast tool told foodie foozled" ),
	INSERT_ITEMS_LIST( "Start and end of line patern",															"^$",			"^foo\\S*$", "*See sample text", "foolhardier" ),
	INSERT_ITEMS_LIST( "Windows return/new line",																"\\r\\n",		"", "", "" ),
	INSERT_ITEMS_LIST( "UNIX new line",																			"\\n",			"", "", "" ),
	//Item starting with 1,2,or 3 in the second character of the second column, are patterns which can only be
	//used for replacement regex field, and when double clicked will get inserted into replacement field.
	INSERT_ITEMS_LIST( "\\1 arg func pattern example with replacement pattern in sample column",				"()",			"foo(\\S*)",		"For results, enter expression and replacement pattern, then click Replacement button on default Sample Text.",		"boo\\1" ),
	INSERT_ITEMS_LIST( "\\2 arg func pattern example with replacement pattern in sample column",				"()()",			"(foo)(\\S*)",		"For results, enter expression and replacement pattern, then click Replacement button on default Sample Text.",		"\\2abc\\1ed" ),
	INSERT_ITEMS_LIST( "\\3 arg func pattern example with replacement pattern in sample column",				"()()()",		"(f)(oo)(\\S*)",	"For results, enter expression and replacement pattern, then click Replacement button on default Sample Text.",		"z\\1aa\\3" ),
	INSERT_ITEMS_LIST( "$1 arg func pattern example with replacement pattern in sample column",					"()",			"foo(\\S*)",		"For results, enter expression and replacement pattern, then click Replacement button on default Sample Text.",		"boo$1" ),
	INSERT_ITEMS_LIST( "$2 arg func pattern example with replacement pattern in sample column",					"()()",			"(foo)(\\S*)",		"For results, enter expression and replacement pattern, then click Replacement button on default Sample Text.",		"$2abc$1ed" ),
	INSERT_ITEMS_LIST( "$3 arg func pattern example with replacement pattern in sample column",					"()()()",		"(f)(oo)(\\S*)",	"For results, enter expression and replacement pattern, then click Replacement button on default Sample Text.",		"z$1aa$3" ),
	INSERT_ITEMS_LIST( "*\\2 arg func pattern example with a to-upper case pattern in sample column",			"\\U\\1\\E",	"(foo)(\\S*)",		"For results, enter expression and replacement pattern, then click Replacement button on default Sample Text.",		"\\U\\1\\E\\2" ),
	INSERT_ITEMS_LIST( "*$2 arg func pattern example with a to-upper case pattern in sample column",			"\\U$1\\E",		"(foo)(\\S*)",		"For results, enter expression and replacement pattern, then click Replacement button on default Sample Text.",		"\\U$1\\E$2" ),
	INSERT_ITEMS_LIST( "*\\2 arg func pattern example with a to-lower case pattern in sample column",			"\\L\\1\\E",	"(foo)(\\S*)",		"For results, enter expression and replacement pattern, then click Replacement button on default Sample Text.",		"\\L\\1\\E\\2" ),
	INSERT_ITEMS_LIST( "*$2 arg func pattern example with a to-lower case pattern in sample column",			"\\L$1\\E",		"(foo)(\\S*)",		"For results, enter expression and replacement pattern, then click Replacement button on default Sample Text.",		"\\L$1\\E$2" ),
	INSERT_ITEMS_LIST( "\\1st Argument for replacement token", "\\1", "", "", "" ),
	INSERT_ITEMS_LIST( "\\2nd Argument for replacement token", "\\2", "", "", "" ),
	INSERT_ITEMS_LIST( "\\3rd Argument for replacement token", "\\3", "", "", "" ),
	INSERT_ITEMS_LIST( "$1st Argument for replacement token", "$1", "", "", "" ),
	INSERT_ITEMS_LIST( "$2nd Argument for replacement token", "$2", "", "", "" ),
	INSERT_ITEMS_LIST( "$3rd Argument for replacement token", "$3", "", "", "" ),
	INSERT_ITEMS_LIST( "$Alternate syntax for replacement token", "$1($2)", "", "", "" ),
	INSERT_ITEMS_LIST( "(Win-Local) File name pattern",															"[a-zA-Z]:\\\\[0-9a-zA-Z\\\\\\s\\-_]+\\.[0-9a-zA-Z_]+", "", "C:\\program\\filename.txt, d:\\foofoo.doc", "" ),
	INSERT_ITEMS_LIST( "(Win-UNC) File name pattern",															"\\\\\\\\[0-9a-zA-Z\\\\\\s\\-_]+\\.[0-9a-zA-Z_]+", "", "\\\\axter\\devl\\dmaisonave\\filename.txt, \\\\DevMain\\MyShare\\foofoo.doc", "" ),
	INSERT_ITEMS_LIST( "(UNIX) File name pattern",																"/[0-9a-zA-Z/\\s\\-_]+\\.[0-9a-zA-Z_]+", "", "/var/log/data.txt", "" ),
	INSERT_ITEMS_LIST( "Email pattern",																			"[0-9a-zA-Z\\-\\s_]+@[0-9a-zA-Z\\-_]+\\.[0-9a-zA-Z]+", "", "foofoo@axter.com, bill@foofoo.com", "" ),
	INSERT_ITEMS_LIST( "GUID pattern",																			"[0-9a-fA-F]+-[0-9a-fA-F]+-[0-9a-fA-F]+-[0-9a-fA-F]+-[0-9a-fA-F]+", "", "E6710055-F6FF-4f60-BF39-EF3372B2A48B", "" ),
	INSERT_ITEMS_LIST( "*Error reporting pattern",																"(?i)fail|error|warn|exception|err\\s?no", "", "Error, Warn, Warning, Exception, exception, FAIL", "" ),
	INSERT_ITEMS_LIST( "*yyyy-mm-dd date format",																"^(19|20)\\d\\d[- /.](0[1-9]|1[012])[- /.](0[1-9]|[12][0-9]|3[01])", "", " 2012-01-31,  1999-12-11,  2008/1/31", "" ),
	INSERT_ITEMS_LIST( "*dd-mm-yyyy date format",																"^(0[1-9]|1[012])[- /.](0[1-9]|[12][0-9]|3[01])[- /.](19|20)\\d\\d", "", "07-03-1961, 21/11/1984, 31-1-2000", "" ),
	INSERT_ITEMS_LIST( "*dd-mm date format",																	"^(0[1-9]|1[012])[- /.](0[1-9]|[12][0-9]|3[01])", "", "07-03, 21/11, 31-1", "" ),
	INSERT_ITEMS_LIST( "*mmm dd, yyyy",																			"[A-Z][a-z][a-z] [0-9][0-9]*, [0-9]{4}", "", "Jan 3, 2003", "" ),
	INSERT_ITEMS_LIST( "*Time patterns",																		"((0?[1-9]|1[012])(\\s*:\\s*([0-5]\\d))?(\\s*:\\s*([0-5]\\d))?(\\s*([AaPp])[Mm]?)$|(2[0-3]|[1]\\d|0?\\d)(\\s*:\\s*([0-5]\\d))(\\s*:\\s*([0-5]\\d))?)", "", "3:00:59 | 12pm | 6:30 a", "" ),
	INSERT_ITEMS_LIST( "*US phone no. pattern [word]",															"\\b([0-9]{3})-([0-9]{3})-([0-9]{4})\\b", "", "111-123-4567", "" ),
	INSERT_ITEMS_LIST( "*US phone no. pattern",																	"((\\(\\d{3}\\))|(\\d{3}-))\\d{3}-\\d{4}", "", "(111)123-4567, 111-123-4567, (111)-123-4567", "" ),
	INSERT_ITEMS_LIST( "*International & Domestic Phone Numbers with Ext",										"([\\+][0-9]{1,3}([ \\.\\-])?)?([\\(]{1}[0-9]{3}[\\)])?([0-9A-Z \\.\\-]{1,32})((x|ext|extension)?[0-9]{1,4}?)", "", "(123)456-7890 | (123)456-7890 x123 | +1 (123)456-7890 | 12 3456 789 0 x1234 | (123)456-7890x123 |(123)456-7890ext123 | (123)456-7890 extension123 | 123.456.7890 | 1234567890 | 1234567 | 12 34 56 78 90 | 12 3 4567 890123 x4567 | +12 3456 7890 | +12 34 56 7890 | +12 3456 7890 | +12 34567890", "" ),
	INSERT_ITEMS_LIST( "*IP pattern",																			"\\b(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\b", "", "192.168.0.3, 255.255.255.255", "" ),
	INSERT_ITEMS_LIST( "*Social security number pattern",														"[0-9]{3}-[0-9]{2}-[0-9]{4}", "", "", "" ),
	INSERT_ITEMS_LIST( "*Float pattern",																		"[-+]?([0-9]+)?\\.[0-9]+", "", "123.456, 0.99, .987", "" ),
	INSERT_ITEMS_LIST( "*Zip code pattern",																		"[0-9]{5}(-[0-9]{4})?", "", "19133, 08015-1234", "" ),
	INSERT_ITEMS_LIST( "*URL pattern",																			"http(s)*://(([\\-\\w]+\\.)+[a-zA-Z]{2,4}.*)", "", "http://axter.com, http://axter.com, https://axter.com", "" ),
	INSERT_ITEMS_LIST( "*FTP pattern",																			"ftp://([\\w](:[\\w]))*(([\\-\\w]+\\.)+[a-zA-Z]{2,4}[/\\w]*)", "", "", "" ),
	INSERT_ITEMS_LIST( "*Domain pattern",																		"[\\-\\w]+\\.)+[a-zA-Z]{2,4}", "", "", "" ),
	INSERT_ITEMS_LIST( "*Major credit card pattern",															"\\d{4}-\\d{4}-\\d{4}-\\d{4}", "", "", "" ),
	INSERT_ITEMS_LIST( "*User name pattern",																	"[\\s\\[][a-zA-Z0-9_-]{3,16}\\\\[a-z0-9_-]{3,16}[\\s\\]]", "", "axter-nj\\david", "" ),
	INSERT_ITEMS_LIST( "*Web color pattern",																	"#?(([fFcC0369])\\2){3}", "", "#FFFFFF | FFCC00 | 003300", "" ),
	INSERT_ITEMS_LIST( "*SQL basic query patterns",																"(?i)(SELECT\\s[\\w\\*\\)\\(\\,\\s]+\\sFROM\\s[\\w]+)|(?i)(UPDATE\\s[\\w]+\\sSET\\s[\\w\\,\\'\\=]+)|(?i)(INSERT\\sINTO\\s[\\d\\w]+[\\s\\w\\d\\)\\(\\,]*\\sVALUES\\s\\([\\d\\w\\'\\,\\)]+)|(?i)(DELETE\\sFROM\\s[\\d\\w\\'\\=]+)", "", "SELECT * FROM TABLE, insert into table, UPDATE TABLE SET FIELD=VALUE WHERE ID_FIELD=VALUE_ID, DELETE FROM TABLE WHERE", "" ),
	INSERT_ITEMS_LIST( "*Collect file pattern",																	"([a-zA-Z]:(\\\\w+)*\\\\[a-zA-Z0_9]+)?.cvf", "", "c:\\Program\\Data\\CollectInc.cvf", "" ),
	INSERT_ITEMS_LIST( "HTML tag pattern",																		"</?[a-zA-Z]+/?>", "", "", "" ),
	INSERT_ITEMS_LIST( "Find programming language function with 1 argument pattern",							"(func_name)\\s*\\(\\s*([0-9a-zA-Z_\"']*)\\s*\\)", "(CreateFoo)\\s*\\(\\s*([0-9a-zA-Z_\"']*)\\s*\\)", "CreateFoo(ArgData)", "" ),
	INSERT_ITEMS_LIST( "Find programming language function with 2 arguments pattern",							"(func_name)\\s*\\(\\s*([0-9a-zA-Z_\"']*)\\s*\\,\\s*([0-9a-zA-Z_\"']*)\\s*\\)", "(CreateFoo)\\s*\\(\\s*([0-9a-zA-Z_\"']*)\\s*\\,\\s*([0-9a-zA-Z_\"']*)\\s*\\)", "CreateFoo(ArgData, arg2)", "" ),
	INSERT_ITEMS_LIST( "Find programming language function with 3 arguments pattern",							"(func_name)\\s*\\(\\s*([0-9a-zA-Z_\"']*)\\s*\\,\\s*([0-9a-zA-Z_\"']*)\\s*\\,\\s*([0-9a-zA-Z_\"']*)\\s*\\)", "(CreateFoo)\\s*\\(\\s*([0-9a-zA-Z_\"']*)\\s*\\,\\s*([0-9a-zA-Z_\"']*)\\s*\\,\\s*([0-9a-zA-Z_\"']*)\\s*\\)", "CreateFoo(ArgData, arg2, arg3)", "" ),
	INSERT_ITEMS_LIST( "Find programming language function with 4 arguments pattern",							"(func_name)\\s*\\(\\s*([0-9a-zA-Z_\"']*)\\s*\\,\\s*([0-9a-zA-Z_\"']*)\\s*\\,\\s*([0-9a-zA-Z_\"']*)\\s*\\,\\s*([0-9a-zA-Z_\"']*)\\s*\\)", "(CreateFoo)\\s*\\(\\s*([0-9a-zA-Z_\"']*)\\s*\\,\\s*([0-9a-zA-Z_\"']*)\\s*\\,\\s*([0-9a-zA-Z_\"']*)\\s*\\,\\s*([0-9a-zA-Z_\"']*)\\s*\\)", "CreateFoo(ArgData, arg2, arg3, arg4)", "" ),
	INSERT_ITEMS_LIST( "Find programming language function with 5 arguments pattern",							"(func_name)\\s*\\(\\s*([0-9a-zA-Z_\"']*)\\s*\\,\\s*([0-9a-zA-Z_\"']*)\\s*\\,\\s*([0-9a-zA-Z_\"']*)\\s*\\,\\s*([0-9a-zA-Z_\"']*)\\s*\\,\\s*([0-9a-zA-Z_\"']*)\\s*\\)", "(CreateFoo)\\s*\\(\\s*([0-9a-zA-Z_\"']*)\\s*\\,\\s*([0-9a-zA-Z_\"']*)\\s*\\,\\s*([0-9a-zA-Z_\"']*)\\s*\\,\\s*([0-9a-zA-Z_\"']*)\\s*\\,\\s*([0-9a-zA-Z_\"']*)\\s*\\)", "CreateFoo(ArgData, arg2, arg3, arg4, arg5)", "" ),
	INSERT_ITEMS_LIST( "", "", "", "", "" )
	//ToDo: Check if Lookbehind is supported by the BOOST
	//"*Lookbehind", "", "(?<=bar)foo", ""),
	//"*Lookbehind Not", "", "(?<!bar)foo", ""),

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
const int CRegexAssistantDlg::m_MaxViewWidth		= 4000; //32000; //ToDo: Undo this
const int CRegexAssistantDlg::m_ViewWidthAddedPad	= 10;
const int CRegexAssistantDlg::QtyColumnsInLinst		= 5;
const int CRegexAssistantDlg::MaxInsertItemsList	= ((sizeof( InsertItemsList ) / sizeof( InsertItemsList[0] ))) / QtyColumnsInLinst;
const CString CRegexAssistantDlg::m_DefaultTestTargetTextData = _T("\
Paste your sample text here. The text here becomes highlighted when it matches Regex code (Expression Field). By default this program starts up with the [Sample] and [Expression] fields populated, to help the user quickly learn by example.\r\n\
Here is an exercise to get more familiar with the program interface.\r\n\
1. Delete Expression field, and leave the default content in the sample field.\r\n\
2. Type foo		While typing, notice what is getting highlighted in the sample field.\r\n\
3. Leave the cursor at the end, and double click \\S on the token list. Make sure to double click the upper case \\S.  Notice an additional single charcter get highlighted.\r\n\
4. Reposition the cursor to the end, and double click * on the token list. Notice the remaining characters in the words get highlighted.\r\n\
5. Reposition the cursor to the end, and double click $ on the token list. Notice only the matching words that are at the end of a line are highlighted.\r\n\
6. Reposition the cursor to the start, and double click the ^ on the token list. Notice only matching single word lines are highlighted.\r\n\
foolhardier\r\n\
Sample Set[.] abc cba ABC aacc aaaa adcc aece Set[*?+] abc ABC cba aacc aabb aabbbbc Set[|] abc ABC default bcdef abcdefghi 123abc define\r\n\
foosball football Football baseballs bigfoot footballScores foulball footballPlayers footballplayers smallfootball smallfootballplayer\r\n\
foolery frog beach cake ballfoot Fool foothill run fish foul ball film star flight footlights space footstools\r\n\
Footnotes said the footdraggers took Footage of a foolish footman as he foozled the putt playing golf in a football field\r\n\
cat8fish, cat9food, cat2food cat8fish cat fish cat9food cat11food cat2food cat food | cat8fish cat fish cat9food cat-fall cat2food cat_food catfood catyfoo\r\n\
cat8fish cat fish catAfood cat-fall cat.found cat_food catfood cat&food | catAfood cat fish catzfood cat-fall cat2food cat_food catfood catifoo\r\n\
acabc cba ABC aacca  | Fool foot foot-ball foothill ballfoot football Footnotes\r\n\
David posted file c:\\foothill.doc to site http://www.foodie.com and put a copy in the footlocker\r\n\
David Maisonave plays football better then Foosball, but he is footloose in zip code 08015-6912\r\n\
The IP address 10.165.132.95 comes from a 19133 zip code area, which is where 800-969-2030 and 555.345.1200 is located.\r\n\
http://axter.com ftp://foozled.com http://www.axter.com axter com ftp://davidjr.axter.com  maisonave-axter-com\r\n\
c:\\film.txt          foozle\r\n\
footlights           David\r\n\
tomfoolery           tom-foolery\r\n\
foodie               Friday\r\n\
More test text below......\r\n\
/sub/subsub/subsubsub/footsteps/footlocker.cvf\r\n\
732-123-4567         http://foolhardy.com\r\n\
800-888-8933         ftp://foozled.com 732-987-6543\r\n\
1-800-325-9345       http://footloose.net\r\n\
10.0.0.123           134-42-0156\r\n\
459-78-2390          10.0.0.101 \r\n\
");
