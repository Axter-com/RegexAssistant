/*
	Copyright (C) 2021 David Maisonave
	The RegexAssistant source code is free software. You can redistribute it and/or modify it under the terms of the GNU General Public License.
	This program is distributed in the hope that it will be useful,	but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
*/
//	ToDo: Finish working on Scintilla drag and drop window logic
//	ToDo: Add toolbar like icons (open, save, save-as, Print, SearchAllInDir, ReplaceAllInDir) to the titlebar
//	ToDo: Add bookmarks functionality
//	ToDo: Add Mutiple marker logic which can be saved
//	ToDo: Add Hide/Unhide token list window
//	ToDo: See if the UNDO button can be made with drown option which has the SQL, FileSys, and Ignore Case options.
#include "pch.h"
//#define PY_SSIZE_T_CLEAN
//#include <Python.h>
#include <afxlayout.h>
#include <boost/regex.hpp>
#include <boost/xpressive/xpressive_fwd.hpp>
#include <regex>
#include "framework.h"
#include "RegexAssistantDlg.h"
#include "RegexAssistant.h"
#include "DlgProxy.h"
#include "afxdialogex.h"
#include "CommonFunctions.h"
#include "MultiMonitors.h"
#include "include/Scintilla.h"
#include <vector>
#include "ClipboardXX.hpp"
#include "CAboutDlg.h"
#define GetNeedle(s, i)   boost::xpressive::sregex::compile(const_cast<char *>((LPCSTR)(s.c_str())),(i ? boost::xpressive::regex_constants::icase : boost::xpressive::regex_constants::ECMAScript))

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#pragma warning(disable:26495)
#pragma warning(disable:6495)

IMPLEMENT_DYNAMIC( CRegexAssistantDlg, CDialogEx );
#include "RegexAssistantDlg.hpp"  //CRegexAssistantDlg static members initialization

using namespace FXString;
using namespace std;

CRegexAssistantDlg::CRegexAssistantDlg( CString regex_search, CString regex_replace, CString Sample, SampleLoadMethod sampleloadmethod,
										int MonitorToDisplay, Regex_Compatibility regex_compatibility, CWnd* pParent /*=nullptr*/ )
	: CDialog( IDD_REGEXASSISTANT_DIALOG, pParent )
	, m_dwLastErr( 0 ), m_IgnoreCase( FALSE ), m_bMakingChangeByReplacementLogic( false ), m_pAutoProxy( NULL )
	, m_OriginalRegexStatement( regex_search ), m_MonitorToDisplay( MonitorToDisplay ), m_SampleLoadMethod( sampleloadmethod )
	, m_SampleText( m_DefaultTestTargetTextData ), m_Regex_Compalibility( regex_compatibility ), m_OriginalPlacement(NULL)
	, m_py_decodelocale( NULL ), m_RegexCompatibility_cmbx( _T( "Items in grey, are awaiting implementation." ), mfcx::DisableColorRefSet(RGB( 105, 105, 105 ), RGB( 169, 169, 169 ) ) )
{
	_ASSERT_EXPR( (sizeof( m_MarkerData ) / sizeof( MARKERDATAtag )) == NUM_OF_MARKERS, _T( "Make sure the number of enums before NUM_OF_MARKERS, is equal to the number of MARKERDATAtag in m_MarkerData" ) );
	//if ( m_bMakingChangeByReplacementLogic )
	//{
	//	m_py_decodelocale = Py_DecodeLocale( __argv[0], 0 );
	//	if ( m_py_decodelocale == NULL )
	//		REPORT_ERR_AND_EXIT( -6, "Fatal error: Py_DecodeLocale failed!" );
	//	Py_SetProgramName( m_py_decodelocale );
	//	Py_Initialize();
	//	PyRun_SimpleString( "from time import time,ctime\n"
	//						"print('Hello World.  Today is', ctime(time()))\n" );
	//	Py_FinalizeEx();
	//}

	if ( sampleloadmethod == SampleLoadMethod::SampleLoadFromCommandLine )
		m_SampleText = Sample;
	else if ( sampleloadmethod == SampleLoadMethod::SampleLoadFromClipboard )
	{
		CClipboardXX clipboard;
		std::string paste_text;
		clipboard >> paste_text;
		m_SampleText = FXString::ToWString( paste_text ).c_str();
	} else if ( sampleloadmethod == SampleLoadMethod::SampleLoadFromFile )
	{
		CStdioFile file;
		bool FileReadSuccess = false;
		if ( file.Open( Sample, CFile::modeRead | CFile::typeText ) )
		{
			const int len = static_cast<int>(file.GetLength());
			if ( len > 2 )
			{
				file.Read( m_SampleText.GetBuffer( len + 1 ), len );
				m_SampleText.SetAt( len, 0 );
				m_SampleText.ReleaseBuffer( len + 1 );
				FileReadSuccess = true;
			}
			file.Close();
		}
		if ( !FileReadSuccess )
			m_SampleText.Format( _T( "Could not open file '%s' or file has less then 3 characters.\r\nAppending default string....\r\n\r\n%s" ), FXString::ToWString( Sample ).c_str(), FXString::ToWString( m_DefaultTestTargetTextData ).c_str() );
	}

	m_OriginalSampleValue = FXString::ToString( m_SampleText.operator LPCWSTR() ).c_str();
	m_hIcon = AfxGetApp()->LoadIcon( IDR_MAINFRAME );
}

BOOL CRegexAssistantDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	ASSERT( (IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX );
	ASSERT( IDM_ABOUTBOX < 0xF000 );
	CMenu* pSysMenu = GetSystemMenu( FALSE );
	if ( pSysMenu != nullptr )
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString( IDS_ABOUTBOX );
		ASSERT( bNameValid );
		if ( !strAboutMenu.IsEmpty() )
		{
			pSysMenu->AppendMenu( MF_SEPARATOR );
			pSysMenu->AppendMenu( MF_STRING, IDM_ABOUTBOX, strAboutMenu );
		}
	}
	SetIcon( m_hIcon, TRUE );			// Set big icon
	SetIcon( m_hIcon, FALSE );		// Set small icon

	if ( m_MonitorToDisplay > 0 )
	{
		MultiMonitors multimonitors;
		multimonitors.MoveWindowToMonitor( this->m_hWnd, static_cast<UINT>(m_MonitorToDisplay) );
	}

	m_LastRegexStatement = m_OriginalRegexStatement;
	m_RegexStatement_editBx.SetWindowText( m_OriginalRegexStatement );
	m_TokenList_list.InsertColumn( IdxRegex, _T( "Regex" ), LVCFMT_LEFT, 100 );
	m_TokenList_list.InsertColumn( IdxDescription, _T( "Description" ), LVCFMT_LEFT, 300 );
	m_TokenList_list.InsertColumn( IdxExample, _T( "Expression" ), LVCFMT_LEFT, 110 );
	m_TokenList_list.InsertColumn( IdxSample, _T( "Sample" ), LVCFMT_LEFT, 260 );
	m_TokenList_list.InsertColumn( IdxMatch, _T( "Match(es)" ), LVCFMT_LEFT, 600 );
	PopulateTokenList();
	UpdateWindowTitle();
	for ( int i = 0; i < sizeof( m_RegexCompatibilityProperties ) / sizeof( RegexCompatibilityProperties ); i++ )
		m_RegexCompatibility_cmbx.AddString( m_RegexCompatibilityProperties[i].Name, m_RegexCompatibilityProperties[i].IsItemEnabled, FXString::ToTString( m_RegexCompatibilityProperties[i].HelpTip.c_str() ), 
											 mfcx::ColorRefSet( m_MarkerData[m_RegexCompatibilityProperties[i].HighLightIndexID].TextColor, m_MarkerData[m_RegexCompatibilityProperties[i].HighLightIndexID].BkColor) );
	m_RegexCompatibility_cmbx.EnableToolTips();
	m_RegexCompatibility_cmbx.SetCurSel( (int)m_Regex_Compalibility );
	m_RegexCompatibility_cmbx.EnableWideStrPopup();
	const int WidthSpace = 2;
	const int HeightSpace = 20;
	const int HeightDiff = (int)(HeightSpace * 1.2);
	RECT rect = {0};
	m_SampleText_Label_static.GetWindowRect( &rect );
	ScreenToClient( &rect );
	HWND NewSciWindow = CreateWindowEx( WS_EX_ACCEPTFILES,
										_T( "Scintilla" ),
										_T( "ScintillaAxter" ),
										WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_CLIPCHILDREN,
										rect.left + WidthSpace,
										rect.top + HeightSpace,
										(rect.right - rect.left) - (WidthSpace * 4),
										(rect.bottom - rect.top) - HeightDiff,
										GetSafeHwnd(),
										NULL,
										AfxGetInstanceHandle(),
										this );
	if ( NewSciWindow == NULL )
	{
		m_dwLastErr = GetLastError();
		REPORT_ERR_AND_EXIT( -4, "Failed to register Scintilla class! Error no [%i].", m_dwLastErr );
	} else
		m_ScintillaWrapper.Init( NewSciWindow );

	if ( m_ScintillaWrapper.IsInit() )
	{
		m_ScintillaWrapper.SendEditor( SCI_SETCARETLINEVISIBLE);
		for ( int i = 0; i < NUM_OF_MARKERS; i++ )
		{
			m_ScintillaWrapper.SendEditor( SCI_STYLESETBACK, i + 1, m_MarkerData[i].BkColor );
			m_ScintillaWrapper.SendEditor( SCI_STYLESETFORE, i + 1, m_MarkerData[i].TextColor );
		}
		m_ScintillaWrapper.SendEditor( SCI_APPENDTEXT, m_SampleText );
		GetDynamicLayout()->AddItem( m_ScintillaWrapper.GetWnd(),
									 CMFCDynamicLayout::MoveNone(),
									 CMFCDynamicLayout::SizeHorizontalAndVertical( 100, 100 ) );
	} else
		REPORT_ERR_AND_EXIT( -5, "Failed to initiate Scintilla." );

	::DragAcceptFiles( NewSciWindow , TRUE);
	m_RegexCompatibility_cmbx.EnableWindow( TRUE );
	m_UndoExpressionChange_btn.EnableWindow( FALSE );
	OnEnChangeRegexEditBox();
	CreateSizeGrip();
	UINT nl;
	if ( AfxGetApp()->GetProfileBinary( _T( "MainFrame" ), _T( "WP" ), (LPBYTE*)&m_OriginalPlacement, &nl ) )
	{
		m_OriginalPlacement->showCmd = (m_OriginalPlacement->showCmd == SW_SHOWMAXIMIZED) ? SW_SHOWMAXIMIZED : SW_SHOWNORMAL;
		SetWindowPlacement( m_OriginalPlacement );
	}
	ShowWindow( SW_SHOWNORMAL );
	if ( !IsWindowVisible() )
		ShowWindow( SW_MAXIMIZE );

	return TRUE;
}

CRegexAssistantDlg::~CRegexAssistantDlg()
{
	if ( m_pAutoProxy != nullptr )
		m_pAutoProxy->m_pDialog = nullptr;
	delete[] m_OriginalPlacement;
}

void CRegexAssistantDlg::CreateSizeGrip()
{
	if ( m_hSizeGrip != NULL )
		return;
	RECT rcClient;
	::GetClientRect( GetSafeHwnd(), &rcClient );
	int cxGrip = ::GetSystemMetrics( SM_CXVSCROLL );
	int cyGrip = ::GetSystemMetrics( SM_CYHSCROLL );
	m_hSizeGrip = ::CreateWindow( _T( "ScrollBar" ), _T( "" ),
								  WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | SBS_SIZEGRIP,
								  rcClient.right - cxGrip, rcClient.bottom - cyGrip, cxGrip, cyGrip,
								  GetSafeHwnd(), NULL, NULL, 0 );
	GetDynamicLayout()->AddItem( m_hSizeGrip,
							CMFCDynamicLayout::MoveHorizontalAndVertical( 100, 100 ),
							CMFCDynamicLayout::SizeNone() );
}

void CRegexAssistantDlg::DoDataExchange( CDataExchange* pDX )
{
	CDialog::DoDataExchange( pDX );
	DDX_Control( pDX, IDC_REGEX_EDIT_BOX, m_RegexStatement_editBx );
	DDX_Control( pDX, IDC_TOKEN_LIST_CTRL, m_TokenList_list );
	DDX_Control( pDX, IDC_IGNORE_CASE_CHECK, m_Case_btn );
	DDX_Control( pDX, IDC_REGEX_COMPATIBILITY_SELECTION_COMBO, m_RegexCompatibility_cmbx );
	DDX_Control( pDX, IDC_CONVERT_SQL_WILD_TO_REGEX_BUTTON, m_ConvertSqlWildToRegex_btn );
	DDX_Control( pDX, IDC_CONVERT_FILESYSTEM_WILD_TO_REGEX_BUTTON, m_ConvertFilesysWildToRegex_btn );
	DDX_Control( pDX, IDC_REGEX_REPLACEWITH_BOX, m_RegexReplacementExpression_editBx );
	DDX_Control( pDX, IDC_REPLACE_BUTTON, m_RunRegexReplacement_btn );
	DDX_Control( pDX, IDC_REPLACEWITH_STATIC, m_RegexReplacementExpression_Label_static );
	DDX_Control( pDX, IDC_REPLACE_UNDO_BUTTON, m_UndoRegexReplacementChanges_btn );
	DDX_Control( pDX, IDC_STATIC_GROUPBOX_TOKEN, m_TokenList_Label_static );
	DDX_Control( pDX, IDC_RESET_SAMPLE, m_ResetSampleContent_btn );
	DDX_Control( pDX, IDC_UNDO_EXPRESSION_CHANGE, m_UndoExpressionChange_btn );
	DDX_Control( pDX, IDC_STATIC_GROUPBOX_TEST_TARGET_TEXT, m_SampleText_Label_static );
}

BEGIN_MESSAGE_MAP( CRegexAssistantDlg, CDialog )
	ON_WM_SYSCOMMAND()
	ON_WM_CLOSE()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED( IDOK, &CRegexAssistantDlg::OnOK )
	ON_BN_CLICKED( IDC_IGNORE_CASE_CHECK, &CRegexAssistantDlg::OnBnClickedIgnoreCaseCheck )
	ON_BN_CLICKED( IDC_CONVERT_SQL_WILD_TO_REGEX_BUTTON, &CRegexAssistantDlg::OnBnClickedConvertSqlWildToRegexButton )
	ON_BN_CLICKED( IDC_CONVERT_FILESYSTEM_WILD_TO_REGEX_BUTTON, &CRegexAssistantDlg::OnBnClickedConvertFilesystemWildToRegexButton )
	ON_EN_CHANGE( IDC_REGEX_EDIT_BOX, &CRegexAssistantDlg::OnEnChangeRegexEditBox )
	ON_CBN_SELCHANGE( IDC_REGEX_COMPATIBILITY_SELECTION_COMBO, &CRegexAssistantDlg::OnCbnSelchangeRegexCompatibilitySelectionCombo )
	ON_BN_CLICKED( IDC_REPLACE_BUTTON, &CRegexAssistantDlg::OnBnClickedReplaceButton )
	ON_BN_CLICKED( IDC_REPLACE_UNDO_BUTTON, &CRegexAssistantDlg::OnBnClickedReplaceUndoButton )
	ON_BN_CLICKED( IDC_STATIC_GROUPBOX_TEST_TARGET_TEXT, &CRegexAssistantDlg::OnBnClickedStaticGroupboxTestTargetText )
	ON_BN_CLICKED( IDC_RESET_SAMPLE, &CRegexAssistantDlg::OnBnClickedResetSample )
	ON_BN_CLICKED( IDC_UNDO_EXPRESSION_CHANGE, &CRegexAssistantDlg::OnBnClickedUndoExpressionChange )
	ON_WM_DROPFILES()
	ON_WM_NOTIFYFORMAT()
	ON_WM_PARENTNOTIFY()
	ON_WM_QUERYOPEN()
	ON_NOTIFY( NM_DBLCLK, IDC_TOKEN_LIST_CTRL, &CRegexAssistantDlg::OnNMDblclkTokenListCtrl )
END_MESSAGE_MAP()

void CRegexAssistantDlg::OnSysCommand( UINT nID, LPARAM lParam )
{
	if ( (nID & 0xFFF0) == IDM_ABOUTBOX )
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	} else
		CDialog::OnSysCommand( nID, lParam );
}

void CRegexAssistantDlg::OnPaint()
{
	if ( IsIconic() )
	{
		CPaintDC dc( this );
		SendMessage( WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0 );
		int cxIcon = GetSystemMetrics( SM_CXICON );
		int cyIcon = GetSystemMetrics( SM_CYICON );
		CRect rect;
		GetClientRect( &rect );
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;
		dc.DrawIcon( x, y, m_hIcon );
	} else
		CDialog::OnPaint();
}

HCURSOR CRegexAssistantDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

BOOL CRegexAssistantDlg::CanExit()
{
	if ( m_pAutoProxy != nullptr )
	{
		ShowWindow( SW_HIDE );
		return FALSE;
	}

	return TRUE;
}

void CRegexAssistantDlg::OnClose()
{
	if ( !CanExit() )
		return;
	if ( m_ScintillaWrapper.IsInit() )
	{
		int BufferSize = m_ScintillaWrapper.SendEditor( SCI_GETTEXTLENGTH ) + 1;
		if ( BufferSize > 0 )
		{
			m_SampleText.Empty();
			std::unique_ptr<char[]> pcBuffer( new char[BufferSize + m_ViewWidthAddedPad]() );
			m_ScintillaWrapper.GetText( BufferSize, pcBuffer );
			m_SampleText = FXString::ToTString( pcBuffer.get() );
		}
	}
	WINDOWPLACEMENT wp;
	GetWindowPlacement( &wp );
	AfxGetApp()->WriteProfileBinary( _T("MainFrame"), _T("WP"), (LPBYTE)&wp, sizeof( wp ) );


	CDialog::OnOK();
}

void CRegexAssistantDlg::UpdateIgnoreCaseStatus( BOOL UpdateButtonStatus, BOOL UpdateExpression )
{
	const CString OrgCurrentText = CopyRegexStatementForUndo();
	CString CurrentText = OrgCurrentText;
	m_IgnoreCase = (m_Case_btn.GetCheck() == BST_CHECKED) ? TRUE : FALSE;
	if ( IsScintilla() && !IsSED() )
	{
		BOOL CurrentIgnoreCaseStatus = m_IgnoreCase;
		if ( CurrentText.Left( 5 ) == _T( "(?-i)" ) )
		{
			CurrentIgnoreCaseStatus = FALSE;
			CurrentText = CurrentText.Mid( 5 );
		}
		if ( CurrentText.Left( 4 ) == _T( "(?i)" ) )
		{
			CurrentIgnoreCaseStatus = TRUE;
			CurrentText = CurrentText.Mid( 4 );
		}

		if ( UpdateButtonStatus && CurrentIgnoreCaseStatus != m_IgnoreCase )
			m_Case_btn.SetCheck((m_IgnoreCase = CurrentIgnoreCaseStatus ));
	}
	else
	{
		if ( UpdateButtonStatus )
		{
			if ( CurrentText.Left( 4 ) == _T( "(?i)" ) )
				m_Case_btn.SetCheck( (m_IgnoreCase = TRUE) );
			if ( CurrentText.Left( 5 ) == _T( "(?-i)" ) )
				m_Case_btn.SetCheck( (m_IgnoreCase = FALSE) );
		}

		if ( m_IgnoreCase )
		{
			if ( CurrentText.Left( 5 ) == _T( "(?-i)" ) )
				CurrentText = _T( "(?i)" ) + CurrentText.Mid( 5 );
			if ( CurrentText.Left( 4 ) != _T( "(?i)" ) )
				CurrentText = _T( "(?i)" ) + CurrentText;
		}
		else
		{
			if ( CurrentText.Left( 4 ) == _T( "(?i)" ) )
				CurrentText = CurrentText.Mid( 4 );
		}

	}

	if ( UpdateExpression && CurrentText != OrgCurrentText )
	{
			m_RegexStatement_editBx.SetWindowText( CurrentText );
			//m_RegexStatement_editBx.SetFocus();
			m_RegexStatement_editBx.SetSel( 0, 0 );
	}
}

void CRegexAssistantDlg::OnBnClickedIgnoreCaseCheck()
{
	UpdateIgnoreCaseStatus( FALSE );
	OnEnChangeRegexEditBox();
}

void CRegexAssistantDlg::OnBnClickedConvertSqlWildToRegexButton()
{
	CString CurrentText;
	m_RegexStatement_editBx.GetWindowText( CurrentText );
	if ( CurrentText.Find( _T( '_' ) ) != -1 || CurrentText.Find( _T( '#' ) ) != -1 || CurrentText.Find( _T( '?' ) ) != -1 || CurrentText.Find( _T( '*' ) ) != -1 || CurrentText.Find( _T( '%' ) ) != -1 )
	{
		CurrentText.Replace( _T( "_" ), _T( "." ) );
		CurrentText.Replace( _T( "#" ), _T( "[0-9]" ) );
		CurrentText.Replace( _T( "?" ), _T( "." ) );
		CurrentText.Replace( _T( "*" ), _T( "\\w*" ) );
		CurrentText.Replace( _T( "\\w**" ), _T( "\\w*" ) );
		CurrentText.Replace( _T( "%" ), _T( ".*" ) );
		m_RegexStatement_editBx.SetWindowText( CurrentText );
		m_RegexStatement_editBx.SetFocus();
		m_RegexStatement_editBx.SetSel( 0, 0 );
	}
}

void CRegexAssistantDlg::OnBnClickedConvertFilesystemWildToRegexButton()
{
	CString CurrentText;
	m_RegexStatement_editBx.GetWindowText( CurrentText );
	if ( CurrentText.Find( _T( '?' ) ) != -1 || CurrentText.Find( _T( '*' ) ) != -1 )
	{
		CurrentText.Replace( _T( "?" ), _T( "." ) );
		CurrentText.Replace( _T( "*" ), _T( ".*" ) );
		CurrentText.Replace( _T( "..*" ), _T( ".*" ) );
		m_RegexStatement_editBx.SetWindowText( CurrentText );
		m_RegexStatement_editBx.SetFocus();
		m_RegexStatement_editBx.SetSel( 0, 0 );
	}
}

int CRegexAssistantDlg::GetMarkerID()
{
	return m_RegexCompatibilityProperties[m_Regex_Compalibility].HighLightIndexID + 1;
}

void CRegexAssistantDlg::ChangeRegexEditBox( CString RegexStatement, DWORD Flag )
{
	long Start = 0, Stop = INT_MAX;
	int Marker = GetMarkerID();
	Sci_TextToFind ft = {0};
	int iLineIndex = 0;

	if ( RegexStatement.IsEmpty() || RegexStatement == _T( "^" ) || RegexStatement == _T( "$" ) )
		return;

	string strMarkText = FXString::ToString( (LPCTSTR)RegexStatement );
	ft.lpstrText = const_cast<char*>(strMarkText.c_str());
	ft.chrg.cpMin = Start;
	ft.chrg.cpMax = Stop;
	bool FoundTextToMark = false;
	int count = 0;
	const int MaxCount = (RegexStatement.GetLength() > 2) ? 32000 : 32;
	while ( m_ScintillaWrapper.SendEditor( SCI_FINDTEXT, Flag, reinterpret_cast<sptr_t>(&ft) ) != -1 )
	{
		if ( ++count > MaxCount )
			break;
		FoundTextToMark = true;
		if ( m_MarkerData[Marker].MarkLine )
		{
			iLineIndex = m_ScintillaWrapper.SendEditor( SCI_LINEFROMPOSITION, ft.chrgText.cpMin );
			ft.chrgText.cpMin = (long)m_ScintillaWrapper.SendEditor( SCI_POSITIONFROMLINE, iLineIndex );
			ft.chrgText.cpMax = (long)m_ScintillaWrapper.SendEditor( SCI_GETLINEENDPOSITION, iLineIndex );
		}

		ft.chrg.cpMin = ft.chrgText.cpMax + 1;

		m_ScintillaWrapper.SetStartStyling( ft, Marker );

		if ( ft.chrg.cpMin >= ft.chrg.cpMax )
			break;
	}
}

void CRegexAssistantDlg::ChangeRegexEditBox_LineByLine( CString RegexStatement )
{
	try
	{
		if ( RegexStatement == _T("\\b") )
			return;
		std::unique_ptr<char[]> pcTempBuffer( new char[m_MaxViewWidth + m_ViewWidthAddedPad]() );
		std::string strRegexStatement = FXString::ToString( RegexStatement.operator LPCWSTR() );
		boost::xpressive::sregex boost_Needle = GetNeedle(strRegexStatement , m_IgnoreCase );
		std::regex std_Needle( strRegexStatement, GetStdRegexOptionFlags() );
		const std::regex_constants::match_flag_type regex_compatibility_flag = GetStdRegexMatchFlags();
		const boost::xpressive::regex_constants::match_flag_type boost_compatibility_flag = GetBoostCompatibilityFlag( m_Regex_Compalibility );
		int count_top = 0;

		for ( int LineCurrent = m_ScintillaWrapper.SendEditor( SCI_LINEFROMPOSITION );; ++LineCurrent )
		{
			count_top++;
			int LineLen = m_ScintillaWrapper.SendEditor( SCI_GETLINE, LineCurrent, reinterpret_cast<sptr_t>(pcTempBuffer.get()) );
			if ( LineLen == 0 || LineLen > m_MaxViewWidth )
				break;
			else
				pcTempBuffer[LineLen] = '\0';
			int count = 0;
			const int MaxCount = 32;
			const long CurrentLineBytePos = (long)m_ScintillaWrapper.SendEditor( SCI_POSITIONFROMLINE, LineCurrent );
			string strHaystack = pcTempBuffer.get();
			string::const_iterator start = strHaystack.begin(), end = strHaystack.end(), itHaystackStart = strHaystack.begin();
			if ( IsBoost() )
			{
				boost::xpressive::smatch what;
				boost::xpressive::regex_constants::match_flag_type flags = boost_compatibility_flag;
				while ( boost::xpressive::regex_search( start, end, what, boost_Needle, flags ) )
				{
					if ( ++count > MaxCount )
						break;
					string::const_iterator NeedleStart = what[0].first;
					string::const_iterator NeedleEnd = what[0].second;
					Sci_TextToFind ft = {0};
					ft.chrgText.cpMin = CurrentLineBytePos + (long)std::distance( itHaystackStart, NeedleStart );
					ft.chrgText.cpMax = CurrentLineBytePos + (long)std::distance( itHaystackStart, NeedleEnd );
					ft.chrg.cpMin = ft.chrgText.cpMax + 1;
					m_ScintillaWrapper.SetStartStyling( ft, GetMarkerID() );

					start = what[0].second;
					flags = boost::xpressive::regex_constants::match_prev_avail | boost_compatibility_flag;
				}
			} else
			{
				std::smatch what;
				while ( std::regex_search( start, end, what, std_Needle, regex_compatibility_flag ) )
				{
					if ( ++count > MaxCount )
						break;
					string::const_iterator NeedleStart = what[0].first;
					string::const_iterator NeedleEnd = what[0].second;
					Sci_TextToFind ft = {0};
					ft.chrgText.cpMin = CurrentLineBytePos + (long)std::distance( itHaystackStart, NeedleStart );
					ft.chrgText.cpMax = CurrentLineBytePos + (long)std::distance( itHaystackStart, NeedleEnd );
					ft.chrg.cpMin = ft.chrgText.cpMax + 1;
					m_ScintillaWrapper.SetStartStyling( ft, GetMarkerID() );
					start = what.suffix().first;
				}
			}
		}
	} catch ( ... )
	{//ToDo: Add logging logic here
		return;
	}
}

void CRegexAssistantDlg::ChangeRegexEditBox_BodyMethod( CString RegexStatement )
{
	std::string strRegexStatement = FXString::ToString( RegexStatement.operator LPCWSTR() );
	int BufferSize = m_ScintillaWrapper.SendEditor( SCI_GETTEXTLENGTH ) + 1;
	if ( BufferSize > 0 )
	{
		std::unique_ptr<char[]> pcBuffer( new char[BufferSize + m_ViewWidthAddedPad]() );
		m_ScintillaWrapper.GetText( BufferSize, pcBuffer );
		try
		{
			const std::regex_constants::match_flag_type regex_compatibility_flag = GetStdRegexMatchFlags();
			const boost::xpressive::regex_constants::match_flag_type boost_compatibility_flag = GetBoostCompatibilityFlag( m_Regex_Compalibility );
			std::regex std_Needle( strRegexStatement, GetStdRegexOptionFlags() );
			boost::xpressive::sregex boost_Needle = GetNeedle( strRegexStatement, m_IgnoreCase );
			string strHaystack = pcBuffer.get();
			string::const_iterator start = strHaystack.begin(), end = strHaystack.end(), itHaystackStart = strHaystack.begin();
			boost::xpressive::regex_constants::match_flag_type flags = boost_compatibility_flag;
			int count = 0;
			const int MaxCount = (RegexStatement.GetLength() > 2) ? 32000 : 32;
			boost::xpressive::smatch boost_what;
			std::smatch std_what;
			while ( IsBoost() ? boost::xpressive::regex_search( start, end, boost_what, boost_Needle, flags ) : std::regex_search( start, end, std_what, std_Needle, regex_compatibility_flag ) )
			{
				if ( ++count > MaxCount )
					break;
				string::const_iterator NeedleStart = IsBoost() ? boost_what[0].first : std_what[0].first;
				string::const_iterator NeedleEnd = IsBoost() ? boost_what[0].second : std_what[0].second;
				Sci_TextToFind ft = {0};
				ft.chrgText.cpMin = (long)std::distance( itHaystackStart, NeedleStart );
				ft.chrgText.cpMax = (long)std::distance( itHaystackStart, NeedleEnd );
				ft.chrg.cpMin = ft.chrgText.cpMax + 1;
				m_ScintillaWrapper.SetStartStyling( ft, GetMarkerID() );
				start = IsBoost() ? boost_what[0].second : std_what.suffix().first;
				if ( IsBoost() )
					flags = boost::xpressive::regex_constants::match_prev_avail | boost_compatibility_flag;
			}
		} catch ( ... )
		{
			//ToDo: Add logging logic here
		}
	}
}

CString CRegexAssistantDlg::CopyRegexStatementForUndo()
{
	CString RegexStatement;
	m_RegexStatement_editBx.GetWindowText( RegexStatement );
	if ( m_LastRegexStatement != RegexStatement)
	{
		if ( m_RegexStatementChangesToUndo.size() > MaxUndoRegex )
			m_RegexStatementChangesToUndo.erase( m_RegexStatementChangesToUndo.begin(), m_RegexStatementChangesToUndo.begin() + 1 );
		m_RegexStatementChangesToUndo.push_back( m_LastRegexStatement );
		m_LastRegexStatement = RegexStatement;
	}

	m_UndoExpressionChange_btn.EnableWindow( m_RegexStatementChangesToUndo.empty() ? FALSE : TRUE );
	return RegexStatement;
}

void CRegexAssistantDlg::OnEnChangeRegexEditBox()
{
	if ( !m_ScintillaWrapper.IsInit())
		return;
	UpdateIgnoreCaseStatus(FALSE);

	CString RegexStatement = CopyRegexStatementForUndo();
	UpdateConversionButtonEnableStatus( RegexStatement );
	if ( RegexStatement.Right( 1 ) == _T( "\\" ) )
	{
		if ( RegexStatement.GetLength() == 1 )
			return;
		if ( RegexStatement.Right( 2 ) != _T( "\\\\" ) )
			return;
	}
	m_ScintillaWrapper.SendEditor( SCI_CLEARDOCUMENTSTYLE );
	//ChangeRegexEditBox( _T( "" ) );		//Call this to make sure previous highlight has been cleared.
	if ( IsBoostOrSTD() )
	{
		if ( RegexStatement.IsEmpty() )
			return;
		if ( IsMultiline() )
			ChangeRegexEditBox_BodyMethod( RegexStatement );
		else
			ChangeRegexEditBox_LineByLine( RegexStatement );
	} else
	{
		DWORD Flag = SCFIND_REGEXP | (IsPOSIX() ? SCFIND_POSIX : 0) | (IsSED() ? SCFIND_CXX11REGEX : 0) | (m_IgnoreCase ? SCFIND_NONE : SCFIND_MATCHCASE);
		ChangeRegexEditBox( RegexStatement, Flag );
	}
}

void CRegexAssistantDlg::UpdateWindowTitle()
{
	SetWindowText( _T( "Regular Expression Assistant (RegexAssistant) by David Maisonave (aka Axter)" ) );
}

void CRegexAssistantDlg::UpdateConversionButtonEnableStatus( CString RegexStatement )
{
	if ( RegexStatement.Find( _T( "#" ) ) == -1 && RegexStatement.Find( _T( "?" ) ) == -1 && RegexStatement.Find( _T( "*" ) ) == -1 && RegexStatement.Find( _T( "%" ) ) == -1 )
	{
		m_ConvertSqlWildToRegex_btn.EnableWindow( FALSE );
		m_ConvertFilesysWildToRegex_btn.EnableWindow( FALSE );
	} else if ( RegexStatement.Find( _T( "?" ) ) == -1 && RegexStatement.Find( _T( "*" ) ) == -1 )
	{
		m_ConvertSqlWildToRegex_btn.EnableWindow( TRUE );
		m_ConvertFilesysWildToRegex_btn.EnableWindow( FALSE );
	} else
	{
		m_ConvertSqlWildToRegex_btn.EnableWindow( TRUE );
		m_ConvertFilesysWildToRegex_btn.EnableWindow( TRUE );
	}
}

void CRegexAssistantDlg::OnCbnSelchangeRegexCompatibilitySelectionCombo()
{
	m_Regex_Compalibility = (Regex_Compatibility)m_RegexCompatibility_cmbx.GetCurSel();
	PopulateTokenList();
	OnEnChangeRegexEditBox();
}

void CRegexAssistantDlg::DragAndDrop( CString Name )
{
	CStdioFile file;

	if ( file.Open( Name, CFile::modeRead | CFile::typeText | CFile::shareDenyNone ) )
	{
		m_SampleText = _T( "" );
		CString Line;
		while ( file.ReadString( Line ) )
			m_SampleText += Line + _T( "\n" );
		if ( m_SampleText.GetLength() > 0 )
		{
			m_ScintillaWrapper.SendEditor( SCI_CLEARALL );
			m_ScintillaWrapper.SendEditor( SCI_APPENDTEXT, m_SampleText );
		}
		file.Close();
	}

}

BOOL CRegexAssistantDlg::OnNotify( WPARAM wParam, LPARAM lParam, LRESULT* pResult )
{

	if ( ((LPNMHDR)lParam)->hwndFrom == m_ScintillaWrapper.GetWnd() )
	{
		SCNotification *pscn = (SCNotification *)lParam;
		if ( pscn->nmhdr.code == SCN_MODIFIED && pscn->text != NULL)
			OnEnChangeRegexEditBox();
		*pResult = 0;
		return TRUE;
	}
	return CWnd::OnNotify( wParam, lParam, pResult );
}

void CRegexAssistantDlg::OnBnClickedReplaceUndoButton()
{
	if ( !m_SampleChangesToUndo.empty() )
	{
		string strHaystack = m_SampleChangesToUndo[m_SampleChangesToUndo.size() - 1];
		m_SampleChangesToUndo.pop_back();
		m_ScintillaWrapper.SendEditor( SCI_CLEARALL );
		m_ScintillaWrapper.SendEditor( SCI_APPENDTEXT, strHaystack );
	}

	if ( m_SampleChangesToUndo.empty() )
	{
		m_UndoRegexReplacementChanges_btn.EnableWindow( FALSE );
	}
}

int CRegexAssistantDlg::GetMaxUndoSampleChange( int SampleSize )
{
	if ( SampleSize < 50000 )
		return MaxUndoSample;
	if ( SampleSize < 500000 )
		return (int)(MaxUndoSample / 10);
	return 2;
}

boost::xpressive::regex_constants::match_flag_type CRegexAssistantDlg::GetBoostCompatibilityFlag( Regex_Compatibility regex_compatibility )
{
	if ( m_RegexCompatibilityProperties[regex_compatibility].CompatibilityAttributes & REGEX_SUBSET_PERL )
		return boost::xpressive::regex_constants::format_perl;

	if ( m_RegexCompatibilityProperties[regex_compatibility].CompatibilityAttributes & REGEX_SUBSET_SED )
		return boost::xpressive::regex_constants::format_sed;

	if ( m_RegexCompatibilityProperties[regex_compatibility].CompatibilityAttributes & REGEX_SUBSET_BOOST_ALL )
		return boost::xpressive::regex_constants::format_all;
	return boost::xpressive::regex_constants::format_default;
}

std::regex_constants::syntax_option_type CRegexAssistantDlg::GetStdRegexOptionFlags()
{
	std::regex_constants::syntax_option_type flag = std::regex_constants::ECMAScript;
	if ( m_RegexCompatibilityProperties[m_Regex_Compalibility].CompatibilityAttributes & REGEX_SUBSET_STD_REGEX_EXT )
		flag |= std::regex_constants::extended;
	else if ( m_RegexCompatibilityProperties[m_Regex_Compalibility].CompatibilityAttributes & REGEX_SUBSET_STD_REGEX_BASIC )
		flag |= std::regex_constants::basic;

	if ( m_IgnoreCase )
		flag |= std::regex_constants::icase;

	return flag;
}

std::regex_constants::match_flag_type CRegexAssistantDlg::GetStdRegexMatchFlags()
{
	std::regex_constants::match_flag_type flag = IsSED() ? std::regex_constants::format_sed : std::regex_constants::match_default;
	return flag;
}

void CRegexAssistantDlg::FetchTextForUndoArray()
{
	try
	{
		int BufferSize = m_ScintillaWrapper.SendEditor( SCI_GETTEXTLENGTH ) + 1;
		if ( BufferSize > 0 )
		{
			std::unique_ptr<char[]> pcBuffer( new char[BufferSize + m_ViewWidthAddedPad]() );
			m_ScintillaWrapper.GetText( BufferSize, pcBuffer);
			if ( m_SampleChangesToUndo.size() > GetMaxUndoSampleChange(BufferSize))
				m_SampleChangesToUndo.erase( m_SampleChangesToUndo.begin(), m_SampleChangesToUndo.begin() + 1 );
			m_SampleChangesToUndo.push_back( pcBuffer.get() );
			m_UndoRegexReplacementChanges_btn.EnableWindow( TRUE );
		}
	} catch ( ... )
	{
	}
}

void CRegexAssistantDlg::OnBnClickedReplaceButton()
{
	if ( !m_ScintillaWrapper.IsInit() )
		return;
	CString NeedleCstr;
	m_RegexStatement_editBx.GetWindowText( NeedleCstr );
	CString NeedleReplacementCstr;
	m_RegexReplacementExpression_editBx.GetWindowText( NeedleReplacementCstr );
	const wstring strHaystack = NeedleReplacementCstr.operator LPCWSTR();
	const boost::match_flag_type flag = boost::format_all;
	const boost::wregex expr{IsNotCompatibleWithBackSlashReplacementToken() ? L"(\\\\)([0-9])" : L"(\\$)([0-9])"};
	if (( IsNotCompatibleWithBackSlashReplacementToken() || IsNotCompatibleWithDollarSignReplacementToken())
		&& boost::regex_search( strHaystack, expr, flag ) )
	{
		wchar_t BadToken	= IsNotCompatibleWithBackSlashReplacementToken() ? '\\' : '$';
		wchar_t GoodToken	= IsNotCompatibleWithBackSlashReplacementToken() ? '$' : '\\';
		CString Msg;
		Msg.Format(_T("Can not use character '%c' for \"%s\" compatibility.  Use character '%c' instead.\nTo replace the characters to correct prefix, answer yes.\nTo continue without correcting text, answer no."), 
					BadToken, m_RegexCompatibilityProperties[m_Regex_Compalibility].Name.operator LPCWSTR(), GoodToken );
		int Answer = MessageBox( Msg, _T( "Wrong argument number prefix charactor" ), MB_YESNOCANCEL );
		if ( Answer == IDCANCEL )
			return;
		if ( Answer == IDYES )
		{
			wstring NeedleReplacement = IsNotCompatibleWithBackSlashReplacementToken() ? L"\\$$2" : L"\\\\$2";
			NeedleReplacementCstr = boost::regex_replace( strHaystack, expr, NeedleReplacement, flag ).c_str();
			m_RegexReplacementExpression_editBx.SetWindowText( NeedleReplacementCstr );
		}
	}

	FetchTextForUndoArray();
	m_bMakingChangeByReplacementLogic = true;
	if ( IsScintilla() )
		RegexReplace_Scintilla( NeedleCstr, NeedleReplacementCstr );
	else if ( IsMultiline() )
		RegexReplace_BodyMethod( NeedleCstr, NeedleReplacementCstr );
	else
		RegexReplace_LineByLine( NeedleCstr, NeedleReplacementCstr );
	m_bMakingChangeByReplacementLogic = false;
}

void CRegexAssistantDlg::RegexReplace_Scintilla( CString Needle, CString NeedleReplacementCstr )
{
	int nCount = 0;
	int lEnd = m_ScintillaWrapper.SendEditor( SCI_GETTEXTLENGTH ) + 1;
	if ( lEnd > 0 )
	{
		//string Needle = FXString::ToString( (LPCTSTR)NeedleCstr );
		string NeedleReplacement = FXString::ToString( (LPCTSTR)NeedleReplacementCstr );
		long lLen = 0;
		long lStart = 0;
		int  flags = SCFIND_REGEXP | (IsPOSIX() ? SCFIND_POSIX : 0) | (IsSED() ? SCFIND_CXX11REGEX : 0) | (m_IgnoreCase ? SCFIND_NONE : SCFIND_MATCHCASE);

		try
		{
			m_ScintillaWrapper.SendEditor( SCI_SETSEARCHFLAGS, flags );
			m_ScintillaWrapper.SendEditor( SCI_SETTARGETSTART, lStart );
			m_ScintillaWrapper.SendEditor( SCI_SETTARGETEND, lEnd );
			long lPos = m_ScintillaWrapper.SendEditor( SCI_SEARCHINTARGET, Needle );
			while ( lPos < lEnd && lPos >= 0 )
			{
				lLen = m_ScintillaWrapper.SendEditor( SCI_REPLACETARGETRE, NeedleReplacement );
				lEnd = m_ScintillaWrapper.SendEditor( SCI_GETTEXTLENGTH );
				m_ScintillaWrapper.SendEditor( SCI_SETTARGETSTART, lPos + lLen );
				m_ScintillaWrapper.SendEditor( SCI_SETTARGETEND, lEnd );
				lPos = m_ScintillaWrapper.SendEditor( SCI_SEARCHINTARGET, Needle );
				nCount++;
			}
		} catch ( ... )
		{
			//ToDo: Add logging logic here
		}
	}
}

void CRegexAssistantDlg::RegexReplace_BodyMethod( CString NeedleCstr, CString NeedleReplacementCstr )
{
	// ToDo: Add logic to do std::regex replace as well as all types of boost replace
	const std::regex_constants::match_flag_type std_regex_compatibility_flag = GetStdRegexMatchFlags();
	const boost::xpressive::regex_constants::match_flag_type boost_compatibility_flag = GetBoostCompatibilityFlag( m_Regex_Compalibility );
	int BufferSize = m_ScintillaWrapper.SendEditor( SCI_GETTEXTLENGTH ) + 1;
	if ( BufferSize > 0 )
	{
		std::unique_ptr<char[]> pcBuffer( new char[BufferSize + m_ViewWidthAddedPad]() );
		try
		{
			m_ScintillaWrapper.GetText(BufferSize, pcBuffer );
			string strHaystack = pcBuffer.get();
			string NeedleReplacement = FXString::ToString( (LPCTSTR)NeedleReplacementCstr );
			string strHaystackWithNeedleReplacement = boost::xpressive::regex_replace( strHaystack, GetNeedle( FXString::ToString( NeedleCstr.operator LPCWSTR() ) , m_IgnoreCase ), NeedleReplacement, boost_compatibility_flag );
			m_ScintillaWrapper.SendEditor( SCI_CLEARALL );
			m_ScintillaWrapper.SendEditor( SCI_APPENDTEXT, strHaystackWithNeedleReplacement );
		} catch ( ... )
		{
			//ToDo: Add logging logic here
		}
	}
}

void CRegexAssistantDlg::RegexReplace_LineByLine( CString NeedleCstr, CString NeedleReplacementCstr )
{
	const std::regex_constants::match_flag_type std_regex_compatibility_flag = GetStdRegexMatchFlags();
	const boost::xpressive::regex_constants::match_flag_type boost_compatibility_flag = GetBoostCompatibilityFlag( m_Regex_Compalibility );

	int BufferSize = m_ScintillaWrapper.SendEditor( SCI_GETTEXTLENGTH ) + 1;
	if ( BufferSize < 1 )
		return;
	std::unique_ptr<char[]> pcBuffer( new char[BufferSize + m_ViewWidthAddedPad]() );
	m_ScintillaWrapper.GetText( BufferSize, pcBuffer );
	string Haystack = pcBuffer.get();
	pcBuffer.reset( new char[m_MaxViewWidth + m_ViewWidthAddedPad] );
	string HaystackAfterReplacement;
	string NeedleReplacement = FXString::ToString( (LPCTSTR)NeedleReplacementCstr );
	boost::xpressive::sregex BoostNeedle;
	std::regex Needle;
	try
	{
		BoostNeedle = GetNeedle( FXString::ToString( NeedleCstr.operator LPCWSTR() ), m_IgnoreCase );
		string s = FXString::ToString( (LPCTSTR)NeedleCstr );
		Needle = std::regex( s, GetStdRegexOptionFlags() );
	} catch ( ... )
	{
		MessageBox( _T( "Error while trying to compile replacement string.  Check " ) );
		return;
	}
	for ( int LineCurrent = m_ScintillaWrapper.SendEditor( SCI_LINEFROMPOSITION );; ++LineCurrent )
	{
		int LineLen = m_ScintillaWrapper.GetText( LineCurrent, pcBuffer, ScintillaWrapper::Sci_GetLine );
		if ( LineLen <= 0 || LineLen > LineCurrent )
			break;
		try
		{
			string strHaystack = pcBuffer.get();
			string ConvertedString;
			try
			{
				if ( IsSTD_Regex() )
				{
					ConvertedString = std::regex_replace( strHaystack, Needle, NeedleReplacement, std_regex_compatibility_flag );
				} else
				{
					ConvertedString = boost::xpressive::regex_replace( strHaystack, BoostNeedle, NeedleReplacement, boost_compatibility_flag );
				}
			} catch ( ... )
			{
				ConvertedString = strHaystack;
			}
			HaystackAfterReplacement += ConvertedString;
		} catch ( ... )
		{
			//ToDo: Add more catch types here so a more specific error can be logged or display
			break;
		}
	}
	if ( Haystack != HaystackAfterReplacement )
	{
		m_ScintillaWrapper.SendEditor( SCI_CLEARALL );
		m_ScintillaWrapper.SendEditor( SCI_APPENDTEXT, HaystackAfterReplacement );
	}
}

void CRegexAssistantDlg::OnBnClickedStaticGroupboxTestTargetText()
{
	CClipboardXX clipboard;
	std::string paste_text;
	clipboard >> paste_text;
	m_ScintillaWrapper.SendEditor( SCI_SETTEXT, paste_text );
}

bool CRegexAssistantDlg::IsBoost()
{
	return IsBoost( m_Regex_Compalibility );
}

bool CRegexAssistantDlg::IsSTD_Regex()
{
	return IsSTD_Regex( m_Regex_Compalibility );
}

bool CRegexAssistantDlg::IsBoostOrSTD()
{
	return IsBoostOrSTD( m_Regex_Compalibility );
}

bool CRegexAssistantDlg::IsScintilla()
{
	return IsScintilla( m_Regex_Compalibility );
}

bool CRegexAssistantDlg::IsBoost( Regex_Compatibility regex_compalibility )
{
	if ( m_RegexCompatibilityProperties[regex_compalibility].CompatibilityAttributes & REGEX_SUBSET_BOOST )
		return true;
	return false;
}

bool CRegexAssistantDlg::IsSTD_Regex( Regex_Compatibility regex_compalibility )
{
	if ( m_RegexCompatibilityProperties[regex_compalibility].CompatibilityAttributes & REGEX_SUBSET_STD_REGEX )
		return true;
	return false;
}

bool CRegexAssistantDlg::IsBoostOrSTD( Regex_Compatibility regex_compalibility )
{
	if ( IsSTD_Regex( regex_compalibility ) )
		return true;
	return IsBoost( regex_compalibility );
}

bool CRegexAssistantDlg::IsScintilla( Regex_Compatibility regex_compalibility )
{
	if ( m_RegexCompatibilityProperties[regex_compalibility].CompatibilityAttributes & REGEX_SUBSET_SCINTILLA )
		return true;
	return false;
}

bool CRegexAssistantDlg::IsNotCompatibleWithBackSlashReplacementToken()
{
	if ( m_RegexCompatibilityProperties[m_Regex_Compalibility].CompatibilityAttributes & REGEX_SUBSET_DOLLAR_PREFIX_ONLY )
		return true;
	return false;
}

bool CRegexAssistantDlg::IsNotCompatibleWithDollarSignReplacementToken()
{
	if ( m_RegexCompatibilityProperties[m_Regex_Compalibility].CompatibilityAttributes & REGEX_SUBSET_BACKSLASH_PREFIX_ONLY )
		return true;
	return false;
}

bool CRegexAssistantDlg::IsPOSIX()
{
	if ( m_RegexCompatibilityProperties[m_Regex_Compalibility].CompatibilityAttributes & REGEX_SUBSET_POSIX )
		return true;
	return false;
}

bool CRegexAssistantDlg::IsSED()
{
	if ( m_RegexCompatibilityProperties[m_Regex_Compalibility].CompatibilityAttributes & REGEX_SUBSET_SED )
		return true;
	return false;
}

bool CRegexAssistantDlg::IsPerl()
{
	if ( m_RegexCompatibilityProperties[m_Regex_Compalibility].CompatibilityAttributes & REGEX_SUBSET_PERL )
		return true;
	return false;
}

bool CRegexAssistantDlg::IsMultiline()
{
	if ( m_RegexCompatibilityProperties[m_Regex_Compalibility].CompatibilityAttributes & REGEX_SUBSET_MULTILINE )
		return true;
	return false;
}

bool CRegexAssistantDlg::IsUNIX_OLD_SYNTAX()
{
	if ( m_RegexCompatibilityProperties[m_Regex_Compalibility].CompatibilityAttributes & REGEX_SUBSET_UNIX_OLD_SYNTAX )
		return true;
	return false;
}

bool CRegexAssistantDlg::IsBoostDefault()
{
	if ( IsBoost() && !IsMultiline() && !IsBoostAll() && !IsSED() )
		return true;
	return false;
}

bool CRegexAssistantDlg::IsBoostAll()
{
	if ( m_RegexCompatibilityProperties[m_Regex_Compalibility].CompatibilityAttributes & REGEX_SUBSET_BOOST_ALL )
		return true;
	return false;
}

bool CRegexAssistantDlg::IsScintillaStandard()
{
	if ( IsScintilla() && IsUNIX_OLD_SYNTAX() )
		return true;
	return false;
}

void CRegexAssistantDlg::PopulateTokenList()
{
	m_TokenList_list.SetRedraw( FALSE );
	m_TokenList_list.DeleteAllItems();
	m_TokenList_list.SetRedraw( TRUE );
	m_TokenListRegexItems.clear();
	m_TokenListExampleRegexItems.clear();
	for ( int i = 0; i < (MaxInsertItemsList - 1); ++i )
	{
		// Use below if conditions to avoid displaying syntax which doesn't apply to the selected compatibility
		CString Description = InsertItemsList[i * QtyColumnsInLinst + IdxDescription];
		bool SkipThisToken = false, FoundToken = true;
		while ( SkipThisToken == false && FoundToken == true )
		{
			FoundToken = false;
			switch ( Description[0] )
			{
			case '*': //Items not compatible with Scintilla except Scintilla SED
				if ( IsScintilla() && !IsSED() )
					SkipThisToken = true;
				else
					FoundToken = true;
				break;
			break; case '@': //Items only compatible with multiline
				if ( !IsMultiline() )
					SkipThisToken = true;
				else
					FoundToken = true;
				break;
			case '<': //Items not compatible with std::regex
				if ( IsSTD_Regex() || IsSED() )
					SkipThisToken = true;
				else
					FoundToken = true;
				break;
			case '\\': // Items only compatible with regex types that support backslash with replacement numbers
				if ( IsNotCompatibleWithBackSlashReplacementToken() )
					SkipThisToken = true;
				else
					FoundToken = true;
				break;
			case '$':// Items only compatible with regex types that support dollar sign with replacement numbers
				if ( IsNotCompatibleWithDollarSignReplacementToken() )
					SkipThisToken = true;
				else
					FoundToken = true;
				break;
			}

			if ( FoundToken )
				Description = Description.Mid( 1 ); // Strip out the prefix character
		}
		
		if ( SkipThisToken )
			continue;

		int nIndex = m_TokenList_list.InsertItem( i, InsertItemsList[i * QtyColumnsInLinst + IdxRegex] );
		m_TokenList_list.SetItemText( nIndex, IdxDescription, Description );
		m_TokenList_list.SetItemText( nIndex, IdxExample, InsertItemsList[i * QtyColumnsInLinst + IdxExample] );
		m_TokenList_list.SetItemText( nIndex, IdxSample, InsertItemsList[i * QtyColumnsInLinst + IdxSample] );
		m_TokenList_list.SetItemText( nIndex, IdxMatch, InsertItemsList[i * QtyColumnsInLinst + IdxMatch] );

		m_TokenListRegexItems.push_back( InsertItemsList[i * QtyColumnsInLinst + IdxRegex] );
		m_TokenListExampleRegexItems.push_back( InsertItemsList[i * QtyColumnsInLinst + IdxExample] );
	}
	m_RunRegexReplacement_btn.EnableWindow( TRUE );
	m_RegexReplacementExpression_editBx.EnableWindow( TRUE );
	m_UndoRegexReplacementChanges_btn.EnableWindow( m_SampleChangesToUndo.empty() ? FALSE : TRUE );
	m_ResetSampleContent_btn.EnableWindow( TRUE );
}

void CRegexAssistantDlg::OnBnClickedResetSample()
{
	m_ScintillaWrapper.SendEditor( SCI_CLEARALL );
	m_ScintillaWrapper.SendEditor( SCI_APPENDTEXT, m_OriginalSampleValue );
}

void CRegexAssistantDlg::OnBnClickedUndoExpressionChange()
{
	if ( m_RegexStatementChangesToUndo.size() )
	{
		m_LastRegexStatement = m_RegexStatementChangesToUndo[m_RegexStatementChangesToUndo.size() - 1];
		m_RegexStatementChangesToUndo.pop_back();
		m_RegexStatement_editBx.SetWindowText( m_LastRegexStatement );
	}
	m_UndoExpressionChange_btn.EnableWindow( m_RegexStatementChangesToUndo.empty() ? FALSE : TRUE );
}

void CRegexAssistantDlg::OnDropFiles( HDROP hdrop )
{
	wchar_t FileName[512] = {0};
	DragQueryFile( hdrop, 0, FileName, (sizeof( FileName )/sizeof(wchar_t))-1 );
	DragAndDrop(FileName);
	CDialog::OnDropFiles( hdrop );
	DragFinish( hdrop );
}

BOOL CRegexAssistantDlg::OnQueryOpen()
{
	// Verify the window is visible, because restoring window placement from previous session can result in placing the window off screen
	if ( !IsWindowVisible() )
		ShowWindow( SW_MAXIMIZE );
	return CDialog::OnQueryOpen();
}

void CRegexAssistantDlg::OnNMDblclkTokenListCtrl( NMHDR *pNMHDR, LRESULT *pResult )
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	int CurSel = m_TokenList_list.GetNextItem( -1, LVNI_SELECTED );
	if ( CurSel != LB_ERR )
	{
		int nStartChar = 0, nEndChar = 0;
		CEdit *EditBoxToChange = &m_RegexStatement_editBx;
		CString InsertStr = m_TokenListRegexItems[CurSel];
		if ( pNMItemActivate->uKeyFlags  & LVKF_CONTROL)
			InsertStr = m_TokenListExampleRegexItems[CurSel];
		if ( InsertStr.GetLength() > 1 && isdigit( InsertStr[1] ) && InsertStr[1] != '0' )
			EditBoxToChange = &m_RegexReplacementExpression_editBx;
		EditBoxToChange->GetSel( nStartChar, nEndChar );
		CString TextToChange;
		EditBoxToChange->GetWindowText( TextToChange );
		if ( pNMItemActivate->uKeyFlags  & LVKF_ALT )
			TextToChange = InsertStr;
		else if ( nStartChar > 0 )
			TextToChange.Insert( nStartChar, InsertStr );
		else
		{
			TextToChange = InsertStr + TextToChange;
			nStartChar = 0;
		}
		EditBoxToChange->SetWindowText( TextToChange );
		EditBoxToChange->SetFocus();
		EditBoxToChange->SetSel( nEndChar + InsertStr.GetLength(), nEndChar + InsertStr.GetLength() );
	}
	*pResult = 0;
}

void CRegexAssistantDlg::KeyUpOrDown( UINT nChar )
{
	if ( nChar == (VK_CONTROL | VK_UP) )
	{
		m_RegexCompatibility_cmbx.VerifyValidSelect( GO_TO_FIRSTVALIDITEM );
	}
	else if ( nChar == (VK_CONTROL | VK_DOWN) )
	{
		m_RegexCompatibility_cmbx.VerifyValidSelect( GO_TO_LASTVALIDITEM );
	}
	else if ( nChar == VK_DOWN )
	{
		m_RegexCompatibility_cmbx.VerifyValidSelect( 1 );
	}
	else if ( nChar == VK_UP )
	{
		m_RegexCompatibility_cmbx.VerifyValidSelect( -1 );
	}
	
	OnCbnSelchangeRegexCompatibilitySelectionCombo();
}

BEGIN_MESSAGE_MAP( CEditGetKeys, CEdit )
	ON_WM_KEYDOWN()
END_MESSAGE_MAP()

void CEditGetKeys::OnKeyDown( UINT nChar, UINT nRepCnt, UINT nFlags )
{
	if ( nChar == VK_DOWN || nChar == VK_UP || nChar == (VK_CONTROL | VK_UP) || nChar == (VK_CONTROL | VK_DOWN) )
	{
		CRegexAssistantDlg* cregexassistantdlg = dynamic_cast<CRegexAssistantDlg*>(GetParent());
		if ( cregexassistantdlg != NULL )
			cregexassistantdlg->KeyUpOrDown( nChar );
	}
	CEdit::OnKeyDown( nChar, nRepCnt, nFlags );
}




