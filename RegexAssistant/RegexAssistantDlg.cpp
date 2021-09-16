/*
	Copyright (C) 2021 David Maisonave
	The RegexAssistant source code is free software. You can redistribute it and/or modify it under the terms of the GNU General Public License.
	This program is distributed in the hope that it will be useful,	but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
*/
#include "pch.h"
//#define PY_SSIZE_T_CLEAN
//#include <Python.h>
#include <afxlayout.h>
#include <boost/regex.hpp>
#include <boost/xpressive/xpressive_fwd.hpp>
#include <regex>
#include "framework.h"
#include "RegexAssistantDlg.h"
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

CRegexAssistantDlg::CRegexAssistantDlg( CString regex_search, CString regex_replace, CString Sample, CRegexAssistantApp::SampleLoadMethod sampleloadmethod,
										int MonitorToDisplay, Regex_Compatibility regex_compatibility, CWnd* pParent /*=nullptr*/ )
	: CDialog( IDD_REGEXASSISTANT_DIALOG, pParent )
	, m_dwLastErr( 0 ), m_IgnoreCase( FALSE ), m_bMakingChangeByReplacementLogic( false ), m_pAutoProxy( NULL )
	, m_OriginalRegexStatement( regex_search ), m_MaxViewWidth( 4000 ), m_MonitorToDisplay( MonitorToDisplay ), m_SampleLoadMethod( sampleloadmethod )
	, m_SampleText( m_DefaultTestTargetTextData ), m_Regex_Compalibility( regex_compatibility )
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

	if ( sampleloadmethod == CRegexAssistantApp::SampleLoadMethod::SampleLoadFromCommandLine )
		m_SampleText = Sample;
	else if ( sampleloadmethod == CRegexAssistantApp::SampleLoadMethod::SampleLoadFromClipboard )
	{
		CClipboardXX clipboard;
		std::string paste_text;
		clipboard >> paste_text;
		m_SampleText = FXString::ToWString( paste_text ).c_str();
	} else if ( sampleloadmethod == CRegexAssistantApp::SampleLoadMethod::SampleLoadFromFile )
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
	m_TokenList_list.InsertColumn( IdxExample, _T( "Example" ), LVCFMT_LEFT, 110 );
	m_TokenList_list.InsertColumn( IdxMatch, _T( "Match(es)" ), LVCFMT_LEFT, 260 );
	PopulateTokenList();
	if ( m_IgnoreCase ) //Only boost multiline supports "(?i)", so it should be safer to have this set to false on original pass
		m_Case_btn.SetCheck( BST_CHECKED );
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
	HWND NewSciWindow = CreateWindowEx( 0,
										_T( "Scintilla" ),
										_T( "" ),
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
		m_ScintillaWrapper.SetPersistentSetings();
		m_ScintillaWrapper.SendEditor( SCI_SETCARETLINEVISIBLE, FALSE );
		for ( int i = 0; i < NUM_OF_MARKERS; i++ )
		{
			m_ScintillaWrapper.SendEditor( SCI_STYLESETBACK, i + 1, m_MarkerData[i].BkColor );
			m_ScintillaWrapper.SendEditor( SCI_STYLESETFORE, i + 1, m_MarkerData[i].TextColor );
		}
		string strText = FXString::ToString( (LPCTSTR)m_SampleText );
		m_ScintillaWrapper.SendEditor( SCI_APPENDTEXT, strText.length(), reinterpret_cast<sptr_t>(strText.c_str()) );
		GetDynamicLayout()->AddItem( m_ScintillaWrapper.GetWnd(),
									 CMFCDynamicLayout::MoveNone(),
									 CMFCDynamicLayout::SizeHorizontalAndVertical( 100, 100 ) );
	} else
		REPORT_ERR_AND_EXIT( -5, "Failed to initiate Scintilla." );

	m_RegexCompatibility_cmbx.EnableWindow( TRUE );
	m_UndoExpressionChange_btn.EnableWindow( FALSE );
	OnEnChangeRegexEditBox();
	CreateSizeGrip();
	ShowWindow( SW_SHOWNORMAL );

	return TRUE;
}

CRegexAssistantDlg::~CRegexAssistantDlg()
{
	if ( m_pAutoProxy != nullptr )
		m_pAutoProxy->m_pDialog = nullptr;
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
	ON_NOTIFY( NM_DBLCLK, IDC_TOKEN_LIST_CTRL, &CRegexAssistantDlg::OnNMDblclkTokenListCtrl )
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
			std::unique_ptr<char[]> pcBuffer( new char[BufferSize + 1]() );
			m_ScintillaWrapper.SendEditor( SCI_GETTEXT, (uptr_t)BufferSize, reinterpret_cast<sptr_t>(pcBuffer.get()) );
			pcBuffer[BufferSize] = '\0';
			m_SampleText = FXString::ToTString( pcBuffer.get() );
		}
	}
	CDialog::OnOK();
}

void CRegexAssistantDlg::OnNMDblclkTokenListCtrl( NMHDR * /*pNMHDR*/, LRESULT *pResult )
{
	int CurSel = m_TokenList_list.GetNextItem( -1, LVNI_SELECTED );
	if ( CurSel != LB_ERR )
	{
		int nStartChar = 0, nEndChar = 0;
		CEdit *EditBoxToChange = &m_RegexStatement_editBx;
		CString InsertStr = m_TokenListRegexItems[CurSel];
		if ( InsertStr.GetLength() > 1 && isdigit( InsertStr[1] ) && InsertStr[1] != '0' )
			EditBoxToChange = &m_RegexReplacementExpression_editBx;
		EditBoxToChange->GetSel( nStartChar, nEndChar );
		CString TextToChange;
		EditBoxToChange->GetWindowText( TextToChange );
		if ( nStartChar > 0 )
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

void CRegexAssistantDlg::UpdateIgnoreCaseStatus( BOOL UpdateButtonStatus)
{
	const CString OrgCurrentText = CopyRegexStatementForUndo();
	CString CurrentText = OrgCurrentText;
	m_IgnoreCase = (m_Case_btn.GetCheck() == BST_CHECKED) ? TRUE : FALSE;
	if ( IsMultiline() )
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
	else
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

	if ( CurrentText != OrgCurrentText )
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

		m_ScintillaWrapper.SendEditor( SCI_STARTSTYLING, ft.chrgText.cpMin, 0x1f );
		m_ScintillaWrapper.SendEditor( SCI_SETSTYLING, ft.chrgText.cpMax - ft.chrgText.cpMin, Marker );

		if ( ft.chrg.cpMin >= ft.chrg.cpMax )
			break;
	}
}

boost::xpressive::regex_constants::match_flag_type CRegexAssistantDlg::GetBoostCompatibilityFlag( Regex_Compatibility regex_compatibility )
{
	if ( m_RegexCompatibilityProperties[regex_compatibility].CompatibilityAttributes & REGEX_SUBSET_BOOST )
	{
		if ( m_RegexCompatibilityProperties[regex_compatibility].CompatibilityAttributes & REGEX_SUBSET_PERL )
			return boost::xpressive::regex_constants::format_perl;

		if ( m_RegexCompatibilityProperties[regex_compatibility].CompatibilityAttributes & REGEX_SUBSET_SED )
			return boost::xpressive::regex_constants::format_sed;

		if ( m_RegexCompatibilityProperties[regex_compatibility].CompatibilityAttributes & REGEX_SUBSET_BOOST_ALL )
			return boost::xpressive::regex_constants::format_all;
		return boost::xpressive::regex_constants::format_default;
	}

	return boost::xpressive::regex_constants::format_all; //REGEX_SUBSET_BOOST_ALL
}

void CRegexAssistantDlg::ChangeRegexEditBox_LineByLine( CString RegexStatement )
{
	try
	{
		if ( RegexStatement == _T("\\b") )
			return;
		std::unique_ptr<char[]> pcTempBuffer( new char[m_MaxViewWidth + 10] );
		std::string strRegexStatement = FXString::ToString( RegexStatement.operator LPCWSTR() );
		boost::xpressive::sregex boost_Needle = GetNeedle(strRegexStatement , m_IgnoreCase );
		std::regex_constants::syntax_option_type default_flag = (m_IgnoreCase ? std::regex_constants::icase : static_cast<std::regex_constants::syntax_option_type>(0));
		std::regex std_Needle( strRegexStatement, default_flag );
		const regex_constants::match_flag_type regex_compatibility_flag = IsSED() ? std::regex_constants::format_sed : std::regex_constants::match_default;
		const boost::xpressive::regex_constants::match_flag_type boost_compatibility_flag = GetBoostCompatibilityFlag( m_Regex_Compalibility );

		for ( int LineCurrent = m_ScintillaWrapper.SendEditor( SCI_LINEFROMPOSITION, 0, 0 );; ++LineCurrent )
		{
			int LineLen = m_ScintillaWrapper.SendEditor( SCI_GETLINE, LineCurrent, reinterpret_cast<sptr_t>(pcTempBuffer.get()) );
			if ( LineLen > 0 )
				pcTempBuffer[LineLen] = '\0';
			else
				break;
			int count = 0;
			const int MaxCount = 32;
			const long CurrentLineBytePos = (long)m_ScintillaWrapper.SendEditor( SCI_POSITIONFROMLINE, LineCurrent );
			string strHaystack = pcTempBuffer.get();
			string::const_iterator start = strHaystack.begin(), end = strHaystack.end(), itHaystackStart = strHaystack.begin();
			if ( IsBoostRegex() )
			{
				boost::xpressive::smatch what;
				boost::xpressive::regex_constants::match_flag_type flags = boost_compatibility_flag;
				while ( boost::xpressive::regex_search( start, end, what, boost_Needle, flags ) )
				{
					if ( ++count > MaxCount )
						break;
					Sci_TextToFind ft = {0};
					string::const_iterator NeedleStart = what[0].first;
					string::const_iterator NeedleEnd = what[0].second;
					ft.chrgText.cpMin = CurrentLineBytePos + (long)std::distance( itHaystackStart, NeedleStart );
					ft.chrgText.cpMax = CurrentLineBytePos + (long)std::distance( itHaystackStart, NeedleEnd );
					ft.chrg.cpMin = ft.chrgText.cpMax + 1;
					m_ScintillaWrapper.SendEditor( SCI_STARTSTYLING, ft.chrgText.cpMin, 0x1f );
					m_ScintillaWrapper.SendEditor( SCI_SETSTYLING, ft.chrgText.cpMax - ft.chrgText.cpMin, GetMarkerID() );

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
					Sci_TextToFind ft = {0};
					string::const_iterator NeedleStart = what[0].first;
					string::const_iterator NeedleEnd = what[0].second;
					ft.chrgText.cpMin = CurrentLineBytePos + (long)std::distance( itHaystackStart, NeedleStart );
					ft.chrgText.cpMax = CurrentLineBytePos + (long)std::distance( itHaystackStart, NeedleEnd );
					ft.chrg.cpMin = ft.chrgText.cpMax + 1;
					m_ScintillaWrapper.SendEditor( SCI_STARTSTYLING, ft.chrgText.cpMin, 0x1f );
					m_ScintillaWrapper.SendEditor( SCI_SETSTYLING, ft.chrgText.cpMax - ft.chrgText.cpMin, GetMarkerID() );
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
	const boost::xpressive::regex_constants::match_flag_type boost_compatibility_flag = GetBoostCompatibilityFlag( m_Regex_Compalibility );
	int BufferSize = m_ScintillaWrapper.SendEditor( SCI_GETTEXTLENGTH ) + 1;
	if ( BufferSize > 0 )
	{
		std::unique_ptr<char[]> pcBuffer( new char[BufferSize + 1] );
		m_ScintillaWrapper.SendEditor( SCI_GETTEXT, (uptr_t)BufferSize, reinterpret_cast<sptr_t>(pcBuffer.get()) );
		pcBuffer[BufferSize] = '\0';
		try
		{
			string strHaystack = pcBuffer.get();
			string::const_iterator start = strHaystack.begin(), end = strHaystack.end(), itHaystackStart = strHaystack.begin();
			boost::xpressive::regex_constants::match_flag_type flags = boost_compatibility_flag;
			int count = 0;
			const int MaxCount = (RegexStatement.GetLength() > 2) ? 32000 : 32;
			boost::xpressive::smatch what;
			while ( boost::xpressive::regex_search( start, end, what, GetNeedle( FXString::ToString( RegexStatement.operator LPCWSTR() ), m_IgnoreCase ), flags ) )
			{
				if ( ++count > MaxCount )
					break;

				Sci_TextToFind ft = {0};
				string::const_iterator NeedleStart = what[0].first;
				string::const_iterator NeedleEnd = what[0].second;
				ft.chrgText.cpMin = (long)std::distance( itHaystackStart, NeedleStart );
				ft.chrgText.cpMax = (long)std::distance( itHaystackStart, NeedleEnd );
				ft.chrg.cpMin = ft.chrgText.cpMax + 1;
				m_ScintillaWrapper.SendEditor( SCI_STARTSTYLING, ft.chrgText.cpMin, 0x1f );
				m_ScintillaWrapper.SendEditor( SCI_SETSTYLING, ft.chrgText.cpMax - ft.chrgText.cpMin, GetMarkerID() );
				start = what[0].second;
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
	UpdateIgnoreCaseStatus();

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
	if ( IsBoostOrStd_Regex() )
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

BOOL CRegexAssistantDlg::OnNotify( WPARAM wParam, LPARAM lParam, LRESULT* pResult )
{
	if ( ((LPNMHDR)lParam)->hwndFrom == m_ScintillaWrapper.GetWnd() )
	{
		SCNotification *pscn = (SCNotification *)lParam;
		*pResult = 0;
		if ( pscn->nmhdr.code == SCN_MODIFIED /*SCN_PAINTED*/ )
			OnEnChangeRegexEditBox();
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
		m_ScintillaWrapper.SendEditor( SCI_APPENDTEXT, strHaystack.size(), reinterpret_cast<sptr_t>(strHaystack.c_str()) );
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

void CRegexAssistantDlg::FetchTextForUndoArray()
{
	try
	{
		int BufferSize = m_ScintillaWrapper.SendEditor( SCI_GETTEXTLENGTH ) + 1;
		if ( BufferSize > 0 )
		{
			std::unique_ptr<char[]> pcBuffer( new char[BufferSize + 1] );
			m_ScintillaWrapper.SendEditor( SCI_GETTEXT, (uptr_t)BufferSize, reinterpret_cast<sptr_t>(pcBuffer.get()) );
			pcBuffer[BufferSize] = '\0';
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

void CRegexAssistantDlg::RegexReplace_Scintilla( CString NeedleCstr, CString NeedleReplacementCstr )
{
	int nCount = 0;
	int lEnd = m_ScintillaWrapper.SendEditor( SCI_GETTEXTLENGTH ) + 1;
	if ( lEnd > 0 )
	{
		string Needle = FXString::ToString( (LPCTSTR)NeedleCstr );
		string NeedleReplacement = FXString::ToString( (LPCTSTR)NeedleReplacementCstr );
		long lLen = 0;
		long lStart = 0;
		int  flags = SCFIND_REGEXP | (IsPOSIX() ? SCFIND_POSIX : 0) | (IsSED() ? SCFIND_CXX11REGEX : 0) | (m_IgnoreCase ? SCFIND_NONE : SCFIND_MATCHCASE);

		try
		{
			m_ScintillaWrapper.SendEditor( SCI_SETSEARCHFLAGS, flags );
			m_ScintillaWrapper.SendEditor( SCI_SETTARGETSTART, lStart );
			m_ScintillaWrapper.SendEditor( SCI_SETTARGETEND, lEnd );
			long lPos = m_ScintillaWrapper.SendEditor( SCI_SEARCHINTARGET, Needle.size(), reinterpret_cast<sptr_t>(Needle.c_str()) );
			while ( lPos < lEnd && lPos >= 0 )
			{
				lLen = m_ScintillaWrapper.SendEditor( SCI_REPLACETARGETRE, NeedleReplacement.size(), reinterpret_cast<sptr_t>(NeedleReplacement.c_str()) );
				lEnd = m_ScintillaWrapper.SendEditor( SCI_GETTEXTLENGTH );
				m_ScintillaWrapper.SendEditor( SCI_SETTARGETSTART, lPos + lLen );
				m_ScintillaWrapper.SendEditor( SCI_SETTARGETEND, lEnd );
				lPos = m_ScintillaWrapper.SendEditor( SCI_SEARCHINTARGET, Needle.size(), reinterpret_cast<sptr_t>(Needle.c_str()) );
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
	const regex_constants::match_flag_type std_regex_compatibility_flag = IsSED() ? std::regex_constants::format_sed : std::regex_constants::match_default;
	const boost::xpressive::regex_constants::match_flag_type boost_compatibility_flag = GetBoostCompatibilityFlag( m_Regex_Compalibility );
	int BufferSize = m_ScintillaWrapper.SendEditor( SCI_GETTEXTLENGTH ) + 1;
	if ( BufferSize > 0 )
	{
		std::unique_ptr<char[]> pcBuffer( new char[BufferSize + 1] );
		m_ScintillaWrapper.SendEditor( SCI_GETTEXT, (uptr_t)BufferSize, reinterpret_cast<sptr_t>(pcBuffer.get()) );
		pcBuffer[BufferSize] = '\0';
		try
		{
			string strHaystack = pcBuffer.get();
			string NeedleReplacement = FXString::ToString( (LPCTSTR)NeedleReplacementCstr );
			string strHaystackWithNeedleReplacement = boost::xpressive::regex_replace( strHaystack, GetNeedle( FXString::ToString( NeedleCstr.operator LPCWSTR() ) , m_IgnoreCase ), NeedleReplacement, boost_compatibility_flag );
			m_ScintillaWrapper.SendEditor( SCI_CLEARALL );
			m_ScintillaWrapper.SendEditor( SCI_APPENDTEXT, strHaystackWithNeedleReplacement.length(), reinterpret_cast<sptr_t>(strHaystackWithNeedleReplacement.c_str()) );
		} catch ( ... )
		{
			//ToDo: Add logging logic here
		}
	}
}

void CRegexAssistantDlg::RegexReplace_LineByLine( CString NeedleCstr, CString NeedleReplacementCstr )
{
	const regex_constants::match_flag_type std_regex_compatibility_flag = IsSED() ? std::regex_constants::format_sed : std::regex_constants::match_default;
	const boost::xpressive::regex_constants::match_flag_type boost_compatibility_flag = GetBoostCompatibilityFlag( m_Regex_Compalibility );

	size_t BufferSize = m_ScintillaWrapper.SendEditor( SCI_GETTEXTLENGTH ) + 1;
	if ( BufferSize < 1 )
		return;
	std::unique_ptr<char[]> pcBuffer( new char[BufferSize + 1] );
	m_ScintillaWrapper.SendEditor( SCI_GETTEXT, (uptr_t)BufferSize, reinterpret_cast<sptr_t>(pcBuffer.get()) );
	pcBuffer[BufferSize] = '\0';
	string Haystack = pcBuffer.get();
	pcBuffer.reset( new char[m_MaxViewWidth + 10] );
	string HaystackAfterReplacement;
	string NeedleReplacement = FXString::ToString( (LPCTSTR)NeedleReplacementCstr );
	boost::xpressive::sregex BoostNeedle;
	std::regex Needle;
	try
	{
		BoostNeedle = GetNeedle( FXString::ToString( NeedleCstr.operator LPCWSTR() ), m_IgnoreCase );
		string s = FXString::ToString( (LPCTSTR)NeedleCstr ).c_str();
		std::regex_constants::syntax_option_type default_flag = (m_IgnoreCase ? std::regex_constants::icase : static_cast<std::regex_constants::syntax_option_type>(0));
		Needle = std::regex( s , default_flag );
	} catch ( ... )
	{
		MessageBox( _T( "Error while trying to compile replacement string.  Check " ) );
		return;
	}
	for ( int LineCurrent = m_ScintillaWrapper.SendEditor( SCI_LINEFROMPOSITION, 0, 0 );; ++LineCurrent )
	{
		int LineLen = m_ScintillaWrapper.SendEditor( SCI_GETLINE, LineCurrent, reinterpret_cast<sptr_t>(pcBuffer.get()) );
		if ( LineLen <= 0 )
			break;
		pcBuffer[LineLen] = '\0';
		try
		{
			string strHaystack = pcBuffer.get();
			string ConvertedString;
			try
			{
				if ( IsStd_Regex() )
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
		m_ScintillaWrapper.SendEditor( SCI_APPENDTEXT, HaystackAfterReplacement.size(), reinterpret_cast<sptr_t>(HaystackAfterReplacement.c_str()) );
	}
}

void CRegexAssistantDlg::OnBnClickedStaticGroupboxTestTargetText()
{
	CClipboardXX clipboard;
	std::string paste_text;
	clipboard >> paste_text;
	m_ScintillaWrapper.SendEditor( SCI_SETTEXT, 0, reinterpret_cast<sptr_t>(paste_text.c_str()) );
}

bool CRegexAssistantDlg::IsBoostRegex()
{
	return IsBoostRegex( m_Regex_Compalibility );
}

bool CRegexAssistantDlg::IsStd_Regex()
{
	return IsStd_Regex( m_Regex_Compalibility );
}

bool CRegexAssistantDlg::IsBoostOrStd_Regex()
{
	return IsBoostOrStd_Regex( m_Regex_Compalibility );
}

bool CRegexAssistantDlg::IsScintilla()
{
	return IsScintilla( m_Regex_Compalibility );
}

bool CRegexAssistantDlg::IsBoostRegex( Regex_Compatibility regex_compalibility )
{
	if ( m_RegexCompatibilityProperties[regex_compalibility].CompatibilityAttributes & REGEX_SUBSET_BOOST )
		return true;
	return false;
}

bool CRegexAssistantDlg::IsStd_Regex( Regex_Compatibility regex_compalibility )
{
	if ( m_RegexCompatibilityProperties[regex_compalibility].CompatibilityAttributes & REGEX_SUBSET_STD_REGEX )
		return true;
	return false;
}

bool CRegexAssistantDlg::IsBoostOrStd_Regex( Regex_Compatibility regex_compalibility )
{
	if ( IsStd_Regex( regex_compalibility ) )
		return true;
	return IsBoostRegex( regex_compalibility );
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

bool CRegexAssistantDlg::IsBoostDefaultRegex()
{
	if ( IsBoostRegex() && !IsMultiline() && !IsBoostAllRegex() && !IsSED() )
		return true;
	return false;
}

bool CRegexAssistantDlg::IsBoostAllRegex()
{
	if ( m_RegexCompatibilityProperties[m_Regex_Compalibility].CompatibilityAttributes & REGEX_SUBSET_BOOST_ALL )
		return true;
	return false;
}

bool CRegexAssistantDlg::IsScintillaStandardRegex()
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
	for ( int i = 0; i < (MaxInsertItemsList - 1); ++i )
	{
		// BOOST_ALL, PERL, and POSIX works with both replacement syntax characters ($ and \)
		// Use below if condition to avoid displaying syntax which doesn't apply to the selected compatibility
		if ( InsertItemsList[i * QtyColumnsInLinst + IdxDescription].Left( 1 ) == "~" ) // Items only compatible with regex types that support backslash with replacement numbers
		{
			if ( IsNotCompatibleWithBackSlashReplacementToken() )
				continue;
		} else if ( InsertItemsList[i * QtyColumnsInLinst + IdxDescription].Left( 1 ) == "$" )// Items only compatible with regex types that support dollar sign with replacement numbers
		{
			if ( IsNotCompatibleWithDollarSignReplacementToken() )
				continue;
		} else if ( InsertItemsList[i * QtyColumnsInLinst + IdxDescription].Left( 1 ) == "*" ) //Items not compatible with Scintilla
		{
			if ( IsScintilla() )
				continue;
		} else if ( InsertItemsList[i * QtyColumnsInLinst + IdxDescription].Left( 1 ) == "#" ) //Items not compatible with std::regex
		{
			if ( IsStd_Regex() || (IsScintilla() && IsSED())) //Scintilla with SED uses std::regex
				continue;
		} else if ( InsertItemsList[i * QtyColumnsInLinst + IdxDescription].Left( 1 ) == "@" ) //Items only compatible with multiline
		{
			if ( !IsMultiline() )
				continue;
		}

		CString Description = InsertItemsList[i * QtyColumnsInLinst + IdxDescription];
		if ( InsertItemsList[i * QtyColumnsInLinst + IdxDescription].Left( 1 ) == "*" || InsertItemsList[i * QtyColumnsInLinst + IdxDescription].Left( 1 ) == "~" || 
			 InsertItemsList[i * QtyColumnsInLinst + IdxDescription].Left( 1 ) == "$" || InsertItemsList[i * QtyColumnsInLinst + IdxDescription].Left( 1 ) == "#" ||
			 InsertItemsList[i * QtyColumnsInLinst + IdxDescription].Left( 1 ) == "@" )
			Description = InsertItemsList[i * QtyColumnsInLinst + IdxDescription].Mid( 1 ); // Strip out the prefix character
		int nIndex = m_TokenList_list.InsertItem( i, InsertItemsList[i * QtyColumnsInLinst + IdxRegex] );
		m_TokenList_list.SetItemText( nIndex, IdxDescription, Description );
		m_TokenList_list.SetItemText( nIndex, IdxExample, InsertItemsList[i * QtyColumnsInLinst + IdxExample] );
		m_TokenList_list.SetItemText( nIndex, IdxMatch, InsertItemsList[i * QtyColumnsInLinst + IdxMatch] );

		m_TokenListRegexItems.push_back( InsertItemsList[i * QtyColumnsInLinst + IdxRegex] );
	}
	m_RunRegexReplacement_btn.EnableWindow( TRUE );
	m_RegexReplacementExpression_editBx.EnableWindow( TRUE );
	m_UndoRegexReplacementChanges_btn.EnableWindow( m_SampleChangesToUndo.empty() ? FALSE : TRUE );
	m_ResetSampleContent_btn.EnableWindow( TRUE );
}

void CRegexAssistantDlg::OnBnClickedResetSample()
{
	m_ScintillaWrapper.SendEditor( SCI_CLEARALL );
	m_ScintillaWrapper.SendEditor( SCI_APPENDTEXT, m_OriginalSampleValue.size(), reinterpret_cast<sptr_t>(m_OriginalSampleValue.c_str()) );
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
