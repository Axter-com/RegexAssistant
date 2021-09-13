/*
	Copyright (C) 2021  David Maisonave
	The RegexAssistant source code is free software. You can redistribute it and/or modify it under the terms of the GNU General Public License.
	This program is distributed in the hope that it will be useful,	but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
*/

#include "pch.h"
//#define PY_SSIZE_T_CLEAN
//#include <Python.h>
#include "framework.h"
#include "RegexAssistant.h"
#include "RegexAssistantDlg.h"
#include "DlgProxy.h"
#include "afxdialogex.h"
#include "CommonFunctions.h"
#include "MultiMonitors.h"
#include "include/Scintilla.h"
#include <vector>

#include "ClipboardXX.hpp"
#include "CAboutDlg.h"
#include <boost/xpressive/xpressive.hpp>
#include <regex>
#define GetNeedle(s)   boost::xpressive::sregex::compile(const_cast<char *>((LPCSTR)(s)))

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
	: CSizingDialog( IDD_REGEXASSISTANT_DIALOG, pParent )
	, m_dwLastErr( 0 ), m_bCase( FALSE ), m_bMakingChangeByReplacementLogic( false ), m_pAutoProxy( NULL )
	, m_CurrentRegexStatement( regex_search ), m_MaxViewWidth( 4000 ), m_MonitorToDisplay( MonitorToDisplay ), m_SampleLoadMethod( sampleloadmethod )
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
	CSizingDialog::OnInitDialog();
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

	SetOrgSizeAsMinSize();
	m_RegexStatement_editBx.SetWindowText( m_CurrentRegexStatement );
	m_TokenList_list.InsertColumn( IdxRegex, _T( "Regex" ), LVCFMT_LEFT, 100 );
	m_TokenList_list.InsertColumn( IdxDescription, _T( "Description" ), LVCFMT_LEFT, 300 );
	m_TokenList_list.InsertColumn( IdxExample, _T( "Example" ), LVCFMT_LEFT, 110 );
	m_TokenList_list.InsertColumn( IdxMatch, _T( "Match(es)" ), LVCFMT_LEFT, 260 );
	PopulateTokenList();
	if ( /*m_FieldId > 0 && */ m_bCase ) //Need to check which compatibility supports "(?i)"
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
	} else
		REPORT_ERR_AND_EXIT( -5, "Failed to initiate Scintilla." );

	m_RegexCompatibility_cmbx.EnableWindow( TRUE );
	OnEnChangeRegexEditBox();
	AddControl( IDC_REGEX_EDIT_BOX, _T( "RX" ) );
	//AddControl( IDC_IGNORE_CASE_CHECK, _T( "RX" ) );
	//AddControl( IDC_CONVERT_SQL_WILD_TO_REGEX_BUTTON, _T( "RX" ) );
	//AddControl( IDC_CONVERT_FILESYSTEM_WILD_TO_REGEX_BUTTON, _T( "RX" ) );
	//AddControl( IDC_REGEX_COMPATIBILITY_SELECTION_COMBO, _T( "RX" ) );
	AddControl( IDC_STATIC_GROUPBOX_TOKEN );
	AddControl( IDC_TOKEN_LIST_CTRL );
	AddControl( IDC_REPLACEWITH_STATIC );
	AddControl( IDC_REGEX_REPLACEWITH_BOX );
	//AddControl( IDC_REPLACE_BUTTON );
	//AddControl( IDC_REPLACE_UNDO_BUTTON );
	AddControl( IDC_STATIC_GROUPBOX_TEST_TARGET_TEXT );
	AddControl( m_ScintillaWrapper.GetWnd() );
	//AddControl( IDC_RESET_SAMPLE );

	ShowWindow( SW_SHOWNORMAL );

	return TRUE;
}

CRegexAssistantDlg::~CRegexAssistantDlg()
{
	if ( m_pAutoProxy != nullptr )
		m_pAutoProxy->m_pDialog = nullptr;
}

void CRegexAssistantDlg::DoDataExchange( CDataExchange* pDX )
{
	CSizingDialog::DoDataExchange( pDX );
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

BEGIN_MESSAGE_MAP( CRegexAssistantDlg, CSizingDialog )
	ON_WM_SYSCOMMAND()
	ON_WM_CLOSE()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED( IDOK, &CRegexAssistantDlg::OnBnClickedOk )
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
		CSizingDialog::OnSysCommand( nID, lParam );
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
		CSizingDialog::OnPaint();
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
	if ( CanExit() )
		CSizingDialog::OnOK();
}

void CRegexAssistantDlg::OnBnClickedOk()
{
	m_RegexStatement_editBx.GetWindowText( m_CurrentRegexStatement );
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
	CSizingDialog::OnOK();
}

void CRegexAssistantDlg::OnNMDblclkTokenListCtrl( NMHDR * /*pNMHDR*/, LRESULT *pResult )
{
	int CurSel = m_TokenList_list.GetNextItem( -1, LVNI_SELECTED );
	if ( CurSel != LB_ERR )
	{
		int nStartChar = 0, nEndChar = 0;
		CEdit *EditBoxToChange = &m_RegexStatement_editBx;
		CString InsertStr = m_RegexList[CurSel];
		if ( InsertStr.GetLength() > 1 && isdigit( InsertStr[1] ) && InsertStr[1] != '0' )
		{
			EditBoxToChange = &m_RegexReplacementExpression_editBx;
		}

		EditBoxToChange->GetSel( nStartChar, nEndChar );
		CString TextToChange;
		EditBoxToChange->GetWindowText( TextToChange );
		if ( nStartChar > 0 )
		{
			TextToChange.Insert( nStartChar, InsertStr );
		} else
		{
			TextToChange = InsertStr + TextToChange;
			nStartChar = 0;
		}
		EditBoxToChange->SetWindowText( TextToChange );
		EditBoxToChange->SetFocus();
		EditBoxToChange->SetSel( nStartChar, nEndChar + InsertStr.GetLength() );
	}
	*pResult = 0;
}

void CRegexAssistantDlg::OnBnClickedIgnoreCaseCheck()
{
	CString CurrentText;
	m_RegexStatement_editBx.GetWindowText( CurrentText );
	if ( m_Case_btn.GetCheck() == BST_CHECKED )
	{
		if ( IsScintillaRegex() )
			m_bCase = TRUE;
		else if ( CurrentText.Left( 4 ) != _T( "(?i)" ) )
		{
			CurrentText = _T( "(?i)" ) + CurrentText;
			m_RegexStatement_editBx.SetWindowText( CurrentText );
			m_RegexStatement_editBx.SetFocus();
			m_RegexStatement_editBx.SetSel( 4, 4 );
		}
	} else
	{
		if ( IsScintillaRegex() )
			m_bCase = FALSE;
		else if ( CurrentText.Left( 4 ) == _T( "(?i)" ) )
		{
			m_RegexStatement_editBx.SetWindowText( CurrentText.Mid( 4 ) );
			m_RegexStatement_editBx.SetFocus();
			m_RegexStatement_editBx.SetSel( 0, 0 );
		}
	}
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

void CRegexAssistantDlg::OnEnChangeRegexEditBox( CString sMarkString, DWORD Flag )
{
	long Start = 0, Stop = INT_MAX;
	int Marker = GetMarkerID();
	TextToFind ft = {0};
	int iLineIndex = 0;
	m_ScintillaWrapper.SendEditor( SCI_CLEARDOCUMENTSTYLE );

	if ( sMarkString.IsEmpty() || sMarkString == _T( "^" ) || sMarkString == _T( "$" ) )
		return;

	string strMarkText = FXString::ToString( (LPCTSTR)sMarkString );
	ft.lpstrText = const_cast<char*>(strMarkText.c_str());
	ft.chrg.cpMin = Start;
	ft.chrg.cpMax = Stop;
	bool FoundTextToMark = false;
	int count = 0;
	const int MaxCount = (sMarkString.GetLength() > 2) ? 32000 : 32;
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
	if ( m_RegexCompatibilityProperties[regex_compatibility].regex_compatibility_basecode & REGEX_SUBSET_BOOST )
	{
		if ( m_RegexCompatibilityProperties[regex_compatibility].regex_compatibility_basecode & REGEX_SUBSET_PERL )
			return boost::xpressive::regex_constants::format_perl;

		if ( m_RegexCompatibilityProperties[regex_compatibility].regex_compatibility_basecode & REGEX_SUBSET_SED )
			return boost::xpressive::regex_constants::format_sed;

		if ( m_RegexCompatibilityProperties[regex_compatibility].regex_compatibility_basecode & REGEX_SUBSET_BOOST_ALL )
			return boost::xpressive::regex_constants::format_all;
		return boost::xpressive::regex_constants::format_default;
	}

	return boost::xpressive::regex_constants::format_all; //REGEX_SUBSET_BOOST_ALL
}

void CRegexAssistantDlg::OnEnChangeRegexEditBox_LineByLine( CString sMarkString )
{
	try
	{
		if ( sMarkString == "\\b" )
			return;
		std::unique_ptr<char[]> pcTempBuffer( new char[m_MaxViewWidth + 10] );
		boost::xpressive::sregex boost_Needle = GetNeedle( FXString::ToString( (LPCTSTR)sMarkString ).c_str() );
		std::regex std_Needle( FXString::ToString( (LPCTSTR)sMarkString ) );
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
					TextToFind ft = {0};
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
					TextToFind ft = {0};
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

void CRegexAssistantDlg::OnEnChangeRegexEditBox_BodyMethod( CString sMarkString )
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
			const int MaxCount = (sMarkString.GetLength() > 2) ? 32000 : 32;
			boost::xpressive::smatch what;
			while ( boost::xpressive::regex_search( start, end, what, GetNeedle( FXString::ToString( (LPCTSTR)sMarkString ).c_str() ), flags ) )
			{
				if ( ++count > MaxCount )
					break;

				TextToFind ft = {0};
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

void CRegexAssistantDlg::OnEnChangeRegexEditBox()
{
	if ( !m_ScintillaWrapper.IsInit() )
		return;
	CString sMarkString;
	m_RegexStatement_editBx.GetWindowText( sMarkString );
	UpdateConversionButtonEnableStatus( sMarkString );
	if ( sMarkString.Right( 1 ) == _T( "\\" ) )
	{
		if ( sMarkString.GetLength() == 1 )
			return;
		if ( sMarkString.Right( 2 ) != _T( "\\\\" ) )
			return;
	}
	if ( IsBoostOrStd_Regex() )
	{
		OnEnChangeRegexEditBox( _T( "" ) );		//Call this to make sure previous highlight has been cleared.
		if ( m_Case_btn.GetCheck() == BST_CHECKED && sMarkString.Left( 4 ) != _T( "(?i)" ) )
		{
			m_Case_btn.SetCheck( BST_UNCHECKED );
		}
		if ( sMarkString.IsEmpty() )
			return;
		if ( IsMultiline() )
			OnEnChangeRegexEditBox_BodyMethod( sMarkString );
		else
			OnEnChangeRegexEditBox_LineByLine( sMarkString );
	} else
	{
		DWORD Flag = SCFIND_REGEXP;
		if ( IsPOSIX() )
			Flag |= SCFIND_POSIX;
		//if ( IsStd_Regex() )
		//	Flag |= SCFIND_CXX11REGEX;  //This version of Scintilla does not have this flag
		if ( m_Case_btn.GetCheck() != BST_CHECKED )
			Flag |= SCFIND_MATCHCASE;
		OnEnChangeRegexEditBox( sMarkString, Flag );
	}
}

void CRegexAssistantDlg::UpdateWindowTitle()
{
	SetWindowText( _T( "Regular Expression Assistant (RegexAssistant) by David Maisonave (aka Axter)" ) );
}

void CRegexAssistantDlg::UpdateConversionButtonEnableStatus( CString sMarkString )
{
	if ( sMarkString.Find( _T( "#" ) ) == -1 && sMarkString.Find( _T( "?" ) ) == -1 && sMarkString.Find( _T( "*" ) ) == -1 && sMarkString.Find( _T( "%" ) ) == -1 )
	{
		m_ConvertSqlWildToRegex_btn.EnableWindow( FALSE );
		m_ConvertFilesysWildToRegex_btn.EnableWindow( FALSE );
	} else if ( sMarkString.Find( _T( "?" ) ) == -1 && sMarkString.Find( _T( "*" ) ) == -1 )
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
	Regex_Compatibility OldRegex_Compalibility = m_Regex_Compalibility;
	m_Regex_Compalibility = (Regex_Compatibility)m_RegexCompatibility_cmbx.GetCurSel();

	if ( IsBoostOrStd_Regex( OldRegex_Compalibility ) && !IsBoostOrStd_Regex() )
	{
		CString sMarkString;
		m_RegexStatement_editBx.GetWindowText( sMarkString );
		if ( sMarkString.Left( 4 ) == _T( "(?i)" ) )
		{
			m_RegexStatement_editBx.SetWindowText( sMarkString.Mid( 4 ) );
			m_RegexStatement_editBx.SetFocus();
			m_RegexStatement_editBx.SetSel( 0, 0 );
		}
	}
	OnBnClickedIgnoreCaseCheck();
	PopulateTokenList();

	OldRegex_Compalibility = m_Regex_Compalibility;
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
	if ( !m_UndoSource.empty() )
	{
		string strHaystack = m_UndoSource[m_UndoSource.size() - 1];
		m_UndoSource.pop_back();
		m_ScintillaWrapper.SendEditor( SCI_CLEARALL );
		m_ScintillaWrapper.SendEditor( SCI_APPENDTEXT, strHaystack.size(), reinterpret_cast<sptr_t>(strHaystack.c_str()) );
	}

	if ( m_UndoSource.empty() )
	{
		m_UndoRegexReplacementChanges_btn.EnableWindow( FALSE );
	}
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
			m_UndoSource.push_back( pcBuffer.get() );
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
	FetchTextForUndoArray();
	CString NeedleCstr;
	m_RegexStatement_editBx.GetWindowText( NeedleCstr );
	CString NeedleReplacementCstr;
	m_RegexReplacementExpression_editBx.GetWindowText( NeedleReplacementCstr );
	m_bMakingChangeByReplacementLogic = true;
	if ( IsScintillaRegex() )
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
		int  flags = IsPOSIX() ? SCFIND_REGEXP | SCFIND_POSIX : SCFIND_REGEXP;

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
			string strHaystackWithNeedleReplacement = boost::xpressive::regex_replace( strHaystack, GetNeedle( FXString::ToString( (LPCTSTR)NeedleCstr ).c_str() ), NeedleReplacement, boost_compatibility_flag );
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
		BoostNeedle = GetNeedle( FXString::ToString( (LPCTSTR)NeedleCstr ).c_str() );
		string s = FXString::ToString( (LPCTSTR)NeedleCstr ).c_str();
		Needle = std::regex( s );
	} catch ( ... )
	{
		//ToDo: Add logging logic here
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
				} else  // Use boost (format_all) as the default method since it works with both $ and \ characters for replace
				{
					ConvertedString = boost::xpressive::regex_replace( strHaystack, BoostNeedle, NeedleReplacement, boost_compatibility_flag );
				}
			} catch ( ... )
			{
				//ToDo: Add logging logic here
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

bool CRegexAssistantDlg::IsScintillaRegex()
{
	return IsScintillaRegex( m_Regex_Compalibility );
}

bool CRegexAssistantDlg::IsBoostRegex( Regex_Compatibility regex_compalibility )
{
	if ( m_RegexCompatibilityProperties[regex_compalibility].regex_compatibility_basecode & REGEX_SUBSET_BOOST )
		return true;
	return false;
}

bool CRegexAssistantDlg::IsStd_Regex( Regex_Compatibility regex_compalibility )
{
	if ( m_RegexCompatibilityProperties[regex_compalibility].regex_compatibility_basecode & REGEX_SUBSET_STD_REGEX )
		return true;
	return false;
}

bool CRegexAssistantDlg::IsBoostOrStd_Regex( Regex_Compatibility regex_compalibility )
{
	if ( IsStd_Regex( regex_compalibility ) )
		return true;
	return IsBoostRegex( regex_compalibility );
}

bool CRegexAssistantDlg::IsScintillaRegex( Regex_Compatibility regex_compalibility )
{
	if ( m_RegexCompatibilityProperties[regex_compalibility].regex_compatibility_basecode & REGEX_SUBSET_SCINTILLA )
		return true;
	return false;
}

bool CRegexAssistantDlg::IsNotCompatibleWithBackSlashReplacementToken()
{
	if ( IsBoostDefaultRegex() )
		return true;
	if ( IsBoostRegex() && IsMultiline() )
		return true;
	if ( IsStd_Regex() && !IsSED() )
		return true;
	return false;
}

bool CRegexAssistantDlg::IsNotCompatibleWithDollarSignReplacementToken()
{
	if ( IsUNIX_OLD_SYNTAX() )
		return true;
	if ( IsPOSIX() || IsSED() )
		return true;
	return false;
}

bool CRegexAssistantDlg::IsPOSIX()
{
	if ( m_RegexCompatibilityProperties[m_Regex_Compalibility].regex_compatibility_basecode & REGEX_SUBSET_POSIX )
		return true;
	return false;
}

bool CRegexAssistantDlg::IsSED()
{
	if ( m_RegexCompatibilityProperties[m_Regex_Compalibility].regex_compatibility_basecode & REGEX_SUBSET_SED )
		return true;
	return false;
}

bool CRegexAssistantDlg::IsPerl()
{
	if ( m_RegexCompatibilityProperties[m_Regex_Compalibility].regex_compatibility_basecode & REGEX_SUBSET_PERL )
		return true;
	return false;
}

bool CRegexAssistantDlg::IsMultiline()
{
	if ( m_RegexCompatibilityProperties[m_Regex_Compalibility].regex_compatibility_basecode & REGEX_SUBSET_MULTILINE )
		return true;
	return false;
}

bool CRegexAssistantDlg::IsUNIX_OLD_SYNTAX()
{
	if ( m_RegexCompatibilityProperties[m_Regex_Compalibility].regex_compatibility_basecode & REGEX_SUBSET_UNIX_OLD_SYNTAX )
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
	if ( m_RegexCompatibilityProperties[m_Regex_Compalibility].regex_compatibility_basecode & REGEX_SUBSET_BOOST_ALL )
		return true;
	return false;
}

bool CRegexAssistantDlg::IsScintillaStandardRegex()
{
	if ( IsScintillaRegex() && IsUNIX_OLD_SYNTAX() )
		return true;
	return false;
}

void CRegexAssistantDlg::PopulateTokenList()
{
	m_TokenList_list.SetRedraw( FALSE );
	m_TokenList_list.DeleteAllItems();
	m_TokenList_list.SetRedraw( TRUE );
	for ( int i = 0; i < (MaxInsertItemsList - 1); ++i )
	{
		//REGEX_COMPATIBILITY_BOOST_ALL, REGEX_COMPATIBILITY_BOOST_PERL, and REGEX_COMPATIBILITY_SCINTILLA_POSIX works with both replacement syntax characters ($ and \)
		//But the remaining only work with one or don't, so use below if condition to avoid display syntax which doesn't apply to the selected compatibility option.
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
			if ( IsScintillaRegex() )
				continue;
		}

		CString Description = InsertItemsList[i * QtyColumnsInLinst + IdxDescription];
		if ( InsertItemsList[i * QtyColumnsInLinst + IdxDescription].Left( 1 ) == "*" || InsertItemsList[i * QtyColumnsInLinst + IdxDescription].Left( 1 ) == "~" || InsertItemsList[i * QtyColumnsInLinst + IdxDescription].Left( 1 ) == "$" )
			Description = InsertItemsList[i * QtyColumnsInLinst + IdxDescription].Mid( 1 );
		int nIndex = m_TokenList_list.InsertItem( i, InsertItemsList[i * QtyColumnsInLinst + IdxRegex] );
		m_TokenList_list.SetItemText( nIndex, IdxDescription, Description );
		m_TokenList_list.SetItemText( nIndex, IdxExample, InsertItemsList[i * QtyColumnsInLinst + IdxExample] );
		m_TokenList_list.SetItemText( nIndex, IdxMatch, InsertItemsList[i * QtyColumnsInLinst + IdxMatch] );

		m_RegexList.push_back( InsertItemsList[i * QtyColumnsInLinst + IdxRegex] );
	}

	//if ( IsScintillaStandardRegex() )
	//{
	//	m_RunRegexReplacement_btn.EnableWindow( FALSE );
	//	m_RegexReplacementExpression_editBx.EnableWindow( FALSE );
	//	m_UndoRegexReplacementChanges_btn.EnableWindow( FALSE );
	//	m_ResetSampleContent_btn.EnableWindow( FALSE );
	//}
	//else
	{
		m_RunRegexReplacement_btn.EnableWindow( TRUE );
		m_RegexReplacementExpression_editBx.EnableWindow( TRUE );
		if ( !m_UndoSource.size() )
			m_UndoRegexReplacementChanges_btn.EnableWindow( FALSE );
		else
			m_UndoRegexReplacementChanges_btn.EnableWindow( TRUE );
		m_ResetSampleContent_btn.EnableWindow( TRUE );
	}

	if ( m_CurrentRegexStatement.Left( 4 ) == "(?i)" )
		m_Case_btn.SetCheck( BST_CHECKED );
}

void CRegexAssistantDlg::OnBnClickedResetSample()
{
	m_ScintillaWrapper.SendEditor( SCI_CLEARALL );
	m_ScintillaWrapper.SendEditor( SCI_APPENDTEXT, m_OriginalSampleValue.size(), reinterpret_cast<sptr_t>(m_OriginalSampleValue.c_str()) );
}

void CRegexAssistantDlg::OnBnClickedUndoExpressionChange()
{
	// TODO: Add your control notification handler code here
}
