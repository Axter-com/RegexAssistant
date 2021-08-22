//Note: This file should only be included in RegexAssistantDlg.cpp
//Initialize static members of CRegexAssistantDlg
MARKERDATAtag CRegexAssistantDlg::m_MarkerData[NUM_OF_MARKERS] = {
	{RGB( 0, 0, 0 )/*Black*/, RGB( 0, 255, 0 )/*Lime*/, FALSE},
{RGB( 0, 0, 0 )/*Black*/, RGB( 255, 255, 0 )/*Yellow*/, FALSE},
{RGB( 0, 0, 205 )/*blue3*/, RGB( 255, 255, 0 )/*Yellow*/, FALSE},
{RGB( 0, 0, 0 )/*Black*/, RGB( 0, 255, 0 )/*Lime*/, FALSE},
{RGB( 0, 0, 0 )/*Black*/, RGB( 0, 255, 0 )/*Lime*/, FALSE},
{RGB( 0, 0, 0 )/*Black*/, RGB( 0, 255, 0 )/*Lime*/, FALSE},
{RGB( 0, 0, 0 )/*Black*/, RGB( 0, 255, 0 )/*Lime*/, FALSE},
{RGB( 0, 0, 0 )/*Black*/, RGB( 0, 255, 255 )/*Cyan*/, FALSE},
{RGB( 0, 0, 0 )/*Black*/, RGB( 0, 255, 255 )/*Cyan*/, FALSE},
{RGB( 0, 0, 0 /*Black*/ ), RGB( 255, 128, 0 )/*Orange*/, FALSE}
};

#define INSERT_ITEMS_LIST( _a, _b, _c, _d )		_T(_b), _T(_a), _T(_c), _T(_d)
const CString CRegexAssistantDlg::InsertItemsList[] =
{
	//Item starting with *, are patterns which can only be used with filter, which uses boost regex logic
	INSERT_ITEMS_LIST( "Any single character", ".", "a.c", "abc, aac, acc, adc, aec, ..." ),
	INSERT_ITEMS_LIST( "Zero or more character(s)", ".*", "comm.*", "commercial ventures, commerce activity" ),
	INSERT_ITEMS_LIST( "Zero or more character(s), no white space", "\\w*", "comm.\\w*", "commercial, commerce, community" ),
	INSERT_ITEMS_LIST( "0 or more of previous expression", "*", "ab*c", "ac, abc, abbc, abbbc, ..." ),
	INSERT_ITEMS_LIST( "1 or more of previous expression", "+", "ab+c", "abc, abbc, abbbc, ..." ),
	INSERT_ITEMS_LIST( "0 or 1 of previous expression", "?", "ab?c", "ac, abc" ),
	INSERT_ITEMS_LIST( "Start of a string", "^", "^abc", "abc, abcdefg, abc123, ..." ),
	INSERT_ITEMS_LIST( "End of a string", "$", "abc$", "abc, endsinabc, 123abc, ..." ),
	INSERT_ITEMS_LIST( "*Word Boundary", "\\b", "\\babc\\b", "abc" ),
	INSERT_ITEMS_LIST( "Word", "\\w+", "\\w+", "abc, defg, foo3, foo_123" ),
	INSERT_ITEMS_LIST( "White space", "\\s", "a\\sc", "a c" ),
	INSERT_ITEMS_LIST( "Matches any non-white-space character", "\\S", "axter\\Scom", "axter.com, axter-com" ),
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
	//Item starting with ~, are patterns which can only be used with Markers, which uses Scintilla regex logic
	//Item starting with $ in second column, are patterns which can only be used for replacement regex field, and when double clicked will get inserted into replacement field.
	INSERT_ITEMS_LIST( "Function with 1 argument replacement pattern", "$\\1(\\2)", "", "" ),
	INSERT_ITEMS_LIST( "Function with 2 arguments replacement pattern", "$\\1(\\2, \\3)", "", "" ),
	INSERT_ITEMS_LIST( "Function with 3 arguments replacement pattern", "$\\1(\\2, \\3, \\4)", "", "" ),
	INSERT_ITEMS_LIST( "Function with 4 arguments replacement pattern", "$\\1(\\2, \\3, \\4, \\5)", "", "" ),
	INSERT_ITEMS_LIST( "Function with 5 arguments replacement pattern", "$\\1(\\2, \\3, \\4, \\5, \\6)", "", "" ),
	INSERT_ITEMS_LIST( "", "", "", "" )
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





/*
ToDo: Add below features
	Feature: Automatically reset window when screen size is 800 x 600
	Feature: Add logic to allow window to shrink down further
	Feature: Add logic to remeber last window size and position (to include monitor)
	Feature: Add Replace support to Scintilla compalibility by using std::regex for replace.
	Feature: Add split line to allow user to change size ratio of the token window against the sample window.
	Feature: Replace Target Text
		Add button that will replace the target text with clipboard content
	Feature: Append Target Text
		Add button that will append the target text field with clipboard content
	Feature: Replace Target Text at startup
		Add command line option that will replace target text with clipboard content when the program starts
	Feature: Command line option for Regex input
		Add command line option to populate the regex input field on program startup
	Feature: command line option to perform a regex replace
		Add command line option to perform a regex replace on a file or multiple files matching DOS wildcards
	Feature: Add additional context menu items to the sample field
		Add context menu item to replace (or append) sample with multiple sets of examples designed for each pattern listed in the token  list
	Feature: Build button
		Add Build button which is disabled until content is entered into regex-input field.
		When build button is selected, display a tree menu with following top level branches:
		Find words
		Find Sentence
			Sub nodes will include the string in regex-input field.
			Find Words
				Find words with "foo"
					Find words starting with "foo"
						Only find words in the begging of a line
						Only find words at the end of a line
						Only find words in the middle of a line
						Only get the part after "foo"  **-->Needs lookahead support
						Find words anywhere
					Find words with "foo" in the middle
					Find words ending with "foo"
					Find words having "foo"
				Find words without "foo"
*/
