#include "pch.h"
#include "framework.h"
#include "RegexAssistant.h"
#include "RegexAssistantDlg.h"
#include "DlgProxy.h"
#include "afxdialogex.h"
#include "CommonFunctions.h"
#include "include/Scintilla.h"
#include <vector>
#include "MultiMonitors.h"
#include "ClipboardXX.hpp"
#include "CAboutDlg.h"
using namespace FXString;
using namespace std;

#define INCLUDE_BOOST_REGEX_AND_RELATED_LOGIC
#define USE_BOOST_XPRESSIVE_REGEX_LOGIC
#ifdef INCLUDE_BOOST_REGEX_AND_RELATED_LOGIC
#ifdef USE_BOOST_XPRESSIVE_REGEX_LOGIC
#include <boost/xpressive/xpressive.hpp>
using namespace boost::xpressive;
typedef boost::xpressive::smatch  RegexWhat;
typedef boost::xpressive::regex_constants::match_flag_type  RegexFlagType;
#define DefaultRegexFlagType boost::xpressive::regex_constants::match_default
#define RegexFlagTypePrevAvail boost::xpressive::regex_constants::match_default | boost::xpressive::regex_constants::match_prev_avail
#define GetNeedle(s)   sregex::compile(const_cast<char *>((LPCSTR)(s)))
#else
#include <boost/regex.hpp>
using namespace boost;
typedef  match_results<std::string::const_iterator> RegexWhat;
typedef match_flag_type  RegexFlagType;
#define DefaultRegexFlagType match_default
#define RegexFlagTypePrevAvail match_default | match_prev_avail
#define GetNeedle(s)  regex(const_cast<char *>((LPCSTR)(s)))
#endif
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#pragma warning(disable:26495)
#pragma warning(disable:6495)

IMPLEMENT_DYNAMIC(CRegexAssistantDlg, CDialogEx);

#include "RegexAssistantDlg.hpp"  //CRegexAssistantDlg static members initialization

CRegexAssistantDlg::CRegexAssistantDlg( CString regex_search, CString regex_replace, CString Sample, CRegexAssistantApp::SampleLoadMethod sampleloadmethod, 
										int MonitorToDisplay, Regex_Compatibility regex_compatibility, CWnd* pParent /*=nullptr*/)
	: CSizingDialog(IDD_REGEXASSISTANT_DIALOG, pParent)
	, m_FieldId( 0 ), lastErr( 0 ), m_bCase( FALSE ), m_EnableRegexCompatibilityOption( TRUE ), m_bMakingChangeByReplacementLogic( false ), m_pAutoProxy(NULL)
	, m_CurrentText( regex_search )	, m_MaxViewWidth( 4000 ), m_MonitorToDisplay( MonitorToDisplay ), m_SampleLoadMethod(sampleloadmethod)
	, m_TestTargetTextData( m_DefaultTestTargetTextData ), m_Regex_Compalibility(regex_compatibility)
{
	if ( sampleloadmethod == CRegexAssistantApp::SampleLoadMethod::SampleLoadFromCommandLine )
		m_TestTargetTextData = Sample;
	else if ( sampleloadmethod == CRegexAssistantApp::SampleLoadMethod::SampleLoadFromClipboard )
	{
		CClipboardXX clipboard;
		std::string paste_text;
		clipboard >> paste_text;
		m_TestTargetTextData = FXString::ToWString( paste_text ).c_str();
	}
	else if ( sampleloadmethod == CRegexAssistantApp::SampleLoadMethod::SampleLoadFromFile )
	{
		CStdioFile file;
		bool FileReadSuccess = false;
		if ( file.Open( Sample, CFile::modeRead | CFile::typeText ) )
		{
			const int len = static_cast<int>(file.GetLength());
			if ( len > 2 )
			{
				file.Read( m_TestTargetTextData.GetBuffer( len + 1 ), len );
				m_TestTargetTextData.SetAt( len, 0 );
				m_TestTargetTextData.ReleaseBuffer( len + 1 );
				FileReadSuccess = true;
			}
			file.Close();
		} 
		if ( !FileReadSuccess )
			m_TestTargetTextData.Format( _T( "Could not open file '%s' or file has less then 3 characters.\r\nAppending default string....\r\n\r\n%s" ), FXString::ToWString( Sample ).c_str() , FXString::ToWString(m_DefaultTestTargetTextData ).c_str() );
	}

	//if ( m_FieldId > 0 )
	//	m_Regex_Compalibility = REGEX_COMPATIBILITY_SCINTILLA_POSIX;
	//else
	//	m_Regex_Compalibility = REGEX_COMPATIBILITY_BOOST;

	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

BOOL CRegexAssistantDlg::OnInitDialog()
{
	CSizingDialog::OnInitDialog();
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);
	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	if ( m_MonitorToDisplay > 0 )
	{
		MultiMonitors multimonitors;
		multimonitors.MoveWindowToMonitor( this->m_hWnd, static_cast<UINT>(m_MonitorToDisplay) );
	}

	SetOrgSizeAsMinSize();
	m_RegexEditBox.SetWindowText( m_CurrentText );
	m_TokenListCtrl.InsertColumn( IdxRegex, _T( "Regex" ), LVCFMT_LEFT, 100 );
	m_TokenListCtrl.InsertColumn( IdxDescription, _T( "Description" ), LVCFMT_LEFT, 300 );
	m_TokenListCtrl.InsertColumn( IdxExample, _T( "Example" ), LVCFMT_LEFT, 110 );
	m_TokenListCtrl.InsertColumn( IdxMatch, _T( "Match(es)" ), LVCFMT_LEFT, 260 );
	for ( int i = 0; i < (MaxInsertItemsList - 1); ++i )
	{
		if ( m_FieldId > 0 && InsertItemsList[i * QtyColumnsInLinst + IdxDescription].Left( 1 ) == "*" )
			continue;
		if ( m_FieldId == 0 && InsertItemsList[i * QtyColumnsInLinst + IdxDescription].Left( 1 ) == "~" )
			continue;

		CString Description = InsertItemsList[i * QtyColumnsInLinst + IdxDescription];
		if ( InsertItemsList[i * QtyColumnsInLinst + IdxDescription].Left( 1 ) == "*" || InsertItemsList[i * QtyColumnsInLinst + IdxDescription].Left( 1 ) == "~" )
			Description = InsertItemsList[i * QtyColumnsInLinst + IdxDescription].Mid( 1 );
		int nIndex = m_TokenListCtrl.InsertItem( i, InsertItemsList[i * QtyColumnsInLinst + IdxRegex] );
		m_TokenListCtrl.SetItemText( nIndex, IdxDescription,  Description);
		m_TokenListCtrl.SetItemText( nIndex, IdxExample, InsertItemsList[i * QtyColumnsInLinst + IdxExample] );
		m_TokenListCtrl.SetItemText( nIndex, IdxMatch, InsertItemsList[i * QtyColumnsInLinst + IdxMatch] );

		m_RegexList.push_back( InsertItemsList[i * QtyColumnsInLinst + IdxRegex] );
	}

	if ( (m_FieldId > 0 && m_bCase) || m_CurrentText.Left( 4 ) == "(?i)" )
		m_Case.SetCheck( BST_CHECKED );
	UpdateWindowTitle();
	m_RegexCompatibility_cmbx.SetCurSel( (int)m_Regex_Compalibility );
	RECT rect = {0};
	m_TokenListCtrl.GetWindowRect( &rect );
	HWND NewSciWindow = CreateWindowEx( 0,
										_T( "Scintilla" ),
										_T( "" ),
										WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_CLIPCHILDREN,
										20,
										470,  //470
										(rect.right - rect.left),
										(rect.bottom - rect.top) + 138, //430
										GetSafeHwnd(),
										NULL,
										AfxGetInstanceHandle(),
										this );
	if ( NewSciWindow == NULL )
	{
		lastErr = GetLastError();
		REPORT_ERR_AND_EXIT(-4, "Failed to register Scintilla class! Error no [%i].", lastErr );
	}
	else
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
		string strText = FXString::ToString( (LPCTSTR)m_TestTargetTextData );
		m_ScintillaWrapper.SendEditor( SCI_APPENDTEXT, strText.length(), reinterpret_cast<sptr_t>(strText.c_str()) );
	} else
		REPORT_ERR_AND_EXIT( -5, "Failed to initiate Scintilla." );

	m_RegexCompatibility_cmbx.EnableWindow( m_EnableRegexCompatibilityOption );
	OnEnChangeRegexEditBox();
	m_ReplaceUndoButton.EnableWindow( FALSE );
	if ( !m_EnableRegexCompatibilityOption || !IsBoostOrStd_Regex( m_Regex_Compalibility ) )
	{
		m_ReplaceWithBox.EnableWindow( FALSE );
		m_ReplaceButton.EnableWindow( FALSE );
		m_ReplaceWithStaticText.EnableWindow( FALSE );
		m_ReplaceUndoButton.EnableWindow( FALSE );
	}

	AddControl( IDC_REGEX_EDIT_BOX, _T( "RX" ) );
	AddControl( IDC_IGNORE_CASE_CHECK, _T( "RX" ) );
	AddControl( IDC_CONVERT_SQL_WILD_TO_REGEX_BUTTON, _T( "RX" ) );
	AddControl( IDC_CONVERT_FILESYSTEM_WILD_TO_REGEX_BUTTON, _T( "RX" ) );
	AddControl( IDC_REGEX_COMPATIBILITY_SELECTION_COMBO, _T( "RX" ) );
	AddControl( IDC_STATIC_GROUPBOX_TOKEN );
	AddControl( IDC_TOKEN_LIST_CTRL );
	AddControl( IDC_REPLACEWITH_STATIC );
	AddControl( IDC_REGEX_REPLACEWITH_BOX );
	AddControl( IDC_REPLACE_BUTTON );
	AddControl( IDC_REPLACE_UNDO_BUTTON );
	AddControl( IDC_STATIC_GROUPBOX_TEST_TARGET_TEXT );
	AddControl( m_ScintillaWrapper.GetWnd() );
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
	DDX_Control( pDX, IDC_REGEX_EDIT_BOX, m_RegexEditBox );
	DDX_Control( pDX, IDC_TOKEN_LIST_CTRL, m_TokenListCtrl );
	DDX_Control( pDX, IDC_IGNORE_CASE_CHECK, m_Case );
	DDX_Control( pDX, IDC_REGEX_COMPATIBILITY_SELECTION_COMBO, m_RegexCompatibility_cmbx );
	DDX_Control( pDX, IDC_CONVERT_SQL_WILD_TO_REGEX_BUTTON, m_ConvertSqlWildToRegex_btn );
	DDX_Control( pDX, IDC_CONVERT_FILESYSTEM_WILD_TO_REGEX_BUTTON, m_ConvertFilesysWildToRegex_btn );
	DDX_Control( pDX, IDC_REGEX_REPLACEWITH_BOX, m_ReplaceWithBox );
	DDX_Control( pDX, IDC_REPLACE_BUTTON, m_ReplaceButton );
	DDX_Control( pDX, IDC_REPLACEWITH_STATIC, m_ReplaceWithStaticText );
	DDX_Control( pDX, IDC_REPLACE_UNDO_BUTTON, m_ReplaceUndoButton );
	DDX_Control( pDX, IDC_STATIC_GROUPBOX_TOKEN, m_GroupBoxToken );
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
END_MESSAGE_MAP()

void CRegexAssistantDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
		CSizingDialog::OnSysCommand(nID, lParam);
}

void CRegexAssistantDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this);
		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
		CSizingDialog::OnPaint();
}

HCURSOR CRegexAssistantDlg::OnQueryDragIcon(){
	return static_cast<HCURSOR>(m_hIcon);
}

BOOL CRegexAssistantDlg::CanExit()
{
	if (m_pAutoProxy != nullptr)
	{
		ShowWindow(SW_HIDE);
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
	m_RegexEditBox.GetWindowText( m_CurrentText );
	if ( m_ScintillaWrapper.IsInit() )
	{
		int BufferSize = m_ScintillaWrapper.SendEditor( SCI_GETTEXTLENGTH ) + 1;
		if ( BufferSize > 0 )
		{
			m_TestTargetTextData.Empty();
			char* pcBuffer = new char[BufferSize + 1]();
			m_ScintillaWrapper.SendEditor( SCI_GETTEXT, (uptr_t)BufferSize, reinterpret_cast<sptr_t>(pcBuffer) );
			pcBuffer[BufferSize] = '\0';
			m_TestTargetTextData = FXString::ToTString( pcBuffer );
			delete[] pcBuffer;
		}
	}
	CSizingDialog::OnOK();
}


void CRegexAssistantDlg::OnNMDblclkTokenListCtrl( NMHDR * /*pNMHDR*/, LRESULT *pResult )
{
	int CurSel = m_TokenListCtrl.GetNextItem( -1, LVNI_SELECTED );
	if ( CurSel != LB_ERR )
	{
		int nStartChar = 0, nEndChar = 0;
		CEdit *EditBoxToChange = &m_RegexEditBox;
		CString InsertStr = m_RegexList[CurSel];
		//ToDo: Add logic to see which field has the focus before uncommenting the below lines of code
		//if ( InsertStr.Left( 1 ) == "$" )
		//{
		//	EditBoxToChange = &m_ReplaceWithBox;
		//	InsertStr = InsertStr.Mid( 1 );
		//}
		EditBoxToChange->GetSel( nStartChar, nEndChar );
		CString CurrentText;
		EditBoxToChange->GetWindowText( CurrentText );
		if ( nStartChar > 0 )
		{
			CurrentText.Insert( nStartChar, InsertStr );
		} else
		{
			CurrentText = InsertStr + CurrentText;
			nStartChar = 0;
		}
		EditBoxToChange->SetWindowText( CurrentText );
		EditBoxToChange->SetFocus();
		EditBoxToChange->SetSel( nStartChar, nEndChar + InsertStr.GetLength() );
	}
	*pResult = 0;
}

void CRegexAssistantDlg::OnBnClickedIgnoreCaseCheck()
{
	CString CurrentText;
	m_RegexEditBox.GetWindowText( CurrentText );
	if ( m_Case.GetCheck() == BST_CHECKED )
	{
		if ( IsScintillaRegex( m_Regex_Compalibility ) )
		{
			m_bCase = TRUE;
		} else if ( CurrentText.Left( 4 ) != _T( "(?i)" ) )
		{
			CurrentText = _T( "(?i)" ) + CurrentText;
			m_RegexEditBox.SetWindowText( CurrentText );
			m_RegexEditBox.SetFocus();
			m_RegexEditBox.SetSel( 4, 4 );
		}
	} else
	{
		if ( IsScintillaRegex( m_Regex_Compalibility ) )
		{
			m_bCase = FALSE;
		} else if ( CurrentText.Left( 4 ) == _T( "(?i)" ) )
		{
			m_RegexEditBox.SetWindowText( CurrentText.Mid( 4 ) );
			m_RegexEditBox.SetFocus();
			m_RegexEditBox.SetSel( 0, 0 );
		}
	}
	OnEnChangeRegexEditBox();
}


void CRegexAssistantDlg::OnBnClickedConvertSqlWildToRegexButton()
{
	CString CurrentText;
	m_RegexEditBox.GetWindowText( CurrentText );
	if ( CurrentText.Find( _T( '_' ) ) != -1 || CurrentText.Find( _T( '#' ) ) != -1 || CurrentText.Find( _T( '?' ) ) != -1 || CurrentText.Find( _T( '*' ) ) != -1 || CurrentText.Find( _T( '%' ) ) != -1 )
	{
		CurrentText.Replace( _T( "_" ), _T( "." ) );
		CurrentText.Replace( _T( "#" ), _T( "[0-9]" ) );
		CurrentText.Replace( _T( "?" ), _T( "." ) );
		CurrentText.Replace( _T( "*" ), _T( "\\w*" ) );
		CurrentText.Replace( _T( "\\w**" ), _T( "\\w*" ) );
		CurrentText.Replace( _T( "%" ), _T( ".*" ) );
		m_RegexEditBox.SetWindowText( CurrentText );
		m_RegexEditBox.SetFocus();
		m_RegexEditBox.SetSel( 0, 0 );
	}
}


void CRegexAssistantDlg::OnBnClickedConvertFilesystemWildToRegexButton()
{
	CString CurrentText;
	m_RegexEditBox.GetWindowText( CurrentText );
	if ( CurrentText.Find( _T( '?' ) ) != -1 || CurrentText.Find( _T( '*' ) ) != -1 )
	{
		CurrentText.Replace( _T( "?" ), _T( "." ) );
		CurrentText.Replace( _T( "*" ), _T( ".*" ) );
		CurrentText.Replace( _T( "..*" ), _T( ".*" ) );
		m_RegexEditBox.SetWindowText( CurrentText );
		m_RegexEditBox.SetFocus();
		m_RegexEditBox.SetSel( 0, 0 );
	}
}

void CRegexAssistantDlg::OnEnChangeRegexEditBox( CString sMarkString, int /*FieldId*/, bool ClearMarkers, DWORD Flag, int TargetLineNo )
{
	long Start = 0, Stop = INT_MAX;
	int Marker = m_Regex_Compalibility; // FieldId - 1;
	TextToFind ft = {0};
	int iLineIndex = 0;
	if ( ClearMarkers )
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

		if ( TargetLineNo == -1 || TargetLineNo == iLineIndex )
		{
			m_ScintillaWrapper.SendEditor( SCI_STARTSTYLING, ft.chrgText.cpMin, 0x1f );
			m_ScintillaWrapper.SendEditor( SCI_SETSTYLING, ft.chrgText.cpMax - ft.chrgText.cpMin, Marker + 1 );
		}

		if ( ft.chrg.cpMin >= ft.chrg.cpMax )
			break;
	}
}

void CRegexAssistantDlg::OnEnChangeRegexEditBox_LineByLine( CString sMarkString )
{
	char* pcTempBuffer = new char[m_MaxViewWidth + 10];
	for ( int LineCurrent = m_ScintillaWrapper.SendEditor( SCI_LINEFROMPOSITION, 0, 0 );; ++LineCurrent )
	{
		int LineLen = m_ScintillaWrapper.SendEditor( SCI_GETLINE, LineCurrent, reinterpret_cast<sptr_t>(pcTempBuffer) );
		if ( LineLen > 0 )
			pcTempBuffer[LineLen] = '\0';
		else
			break;
#ifdef INCLUDE_BOOST_REGEX_AND_RELATED_LOGIC
		try
		{
			RegexWhat what;
			RegexFlagType flags = DefaultRegexFlagType;
			string strHaystack = pcTempBuffer;
			string::const_iterator start = strHaystack.begin(), end = strHaystack.end(), itHaystackStart = strHaystack.begin();
			int count = 0;
			const int MaxCount = 32;
			const long CurrentLineBytePos = (long)m_ScintillaWrapper.SendEditor( SCI_POSITIONFROMLINE, LineCurrent );
			while ( regex_search( start, end, what, GetNeedle( FXString::ToString( (LPCTSTR)sMarkString ).c_str() ), flags ) )
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
				m_ScintillaWrapper.SendEditor( SCI_SETSTYLING, ft.chrgText.cpMax - ft.chrgText.cpMin, m_Regex_Compalibility + 1 );

				start = what[0].second;
				flags = RegexFlagTypePrevAvail;
			}
		} catch ( ... )	{
			break;
		}
#endif
	}
	delete[] pcTempBuffer;
}

void CRegexAssistantDlg::OnEnChangeRegexEditBox_BodyMethod( CString sMarkString )
{
	int BufferSize = m_ScintillaWrapper.SendEditor( SCI_GETTEXTLENGTH ) + 1;
	if ( BufferSize > 0 )
	{
		char* pcBuffer = new char[BufferSize + 1];
		m_ScintillaWrapper.SendEditor( SCI_GETTEXT, (uptr_t)BufferSize, reinterpret_cast<sptr_t>(pcBuffer) );
		pcBuffer[BufferSize] = '\0';
#ifdef INCLUDE_BOOST_REGEX_AND_RELATED_LOGIC
		try
		{
			string strHaystack = pcBuffer;
			string::const_iterator start = strHaystack.begin(), end = strHaystack.end(), itHaystackStart = strHaystack.begin();
			RegexFlagType flags = DefaultRegexFlagType;
			int count = 0;
			const int MaxCount = (sMarkString.GetLength() > 2) ? 32000 : 32;
			RegexWhat what;
			while ( regex_search( start, end, what, GetNeedle( FXString::ToString( (LPCTSTR)sMarkString ).c_str() ), flags ) )
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
				m_ScintillaWrapper.SendEditor( SCI_SETSTYLING, ft.chrgText.cpMax - ft.chrgText.cpMin, m_Regex_Compalibility + 1 );
				start = what[0].second;
				flags = RegexFlagTypePrevAvail;
			}
		} catch ( ... )
		{
			//return;
		}
#endif
		delete[] pcBuffer;
	}
}

void CRegexAssistantDlg::OnEnChangeRegexEditBox()
{
	if ( !m_ScintillaWrapper.IsInit() )
		return;
	CString sMarkString;
	m_RegexEditBox.GetWindowText( sMarkString );
	UpdateConversionButtonEnableStatus( sMarkString );
	if ( sMarkString.Right( 1 ) == _T( "\\" ) )
	{
		if ( sMarkString.GetLength() == 1 )
			return;
		if ( sMarkString.Right( 2 ) != _T( "\\\\" ) )
			return;
	}
	if ( IsBoostOrStd_Regex( m_Regex_Compalibility ) )
	{
		OnEnChangeRegexEditBox( _T( "" ), 1, true );		//Call this to make sure previous highlight has been cleared.
		if ( m_Case.GetCheck() == BST_CHECKED && sMarkString.Left( 4 ) != _T( "(?i)" ) )
		{
			m_Case.SetCheck( BST_UNCHECKED );
		}
		if ( sMarkString.IsEmpty() )
			return;
		if ( m_Regex_Compalibility == REGEX_COMPATIBILITY_BOOST_MULTILINE )
			OnEnChangeRegexEditBox_BodyMethod( sMarkString );
		else
			OnEnChangeRegexEditBox_LineByLine( sMarkString );
	} else
	{
		int FieldId = (m_FieldId > 0) ? m_FieldId : 1;
		DWORD Flag = SCFIND_REGEXP;
		if ( m_Regex_Compalibility == REGEX_COMPATIBILITY_SCINTILLA_POSIX )
			Flag |= SCFIND_POSIX;
		if ( m_Case.GetCheck() != BST_CHECKED )
			Flag |= SCFIND_MATCHCASE;
		OnEnChangeRegexEditBox( sMarkString, FieldId, true, Flag );
	}
}

void CRegexAssistantDlg::UpdateWindowTitle(){
	SetWindowText( _T( "Regular Expression Assistant (RegexAssistant) by David Maisonave (aka Axter)" ) );
}

void CRegexAssistantDlg::UpdateConversionButtonEnableStatus( CString sMarkString )
{
	if ( sMarkString.Find( _T( "#" ) ) == -1 && sMarkString.Find( _T( "?" ) ) == -1 && sMarkString.Find( _T( "*" ) ) == -1  && sMarkString.Find( _T( "%" ) ) == -1 )
	{
		m_ConvertSqlWildToRegex_btn.EnableWindow( FALSE );
		m_ConvertFilesysWildToRegex_btn.EnableWindow( FALSE );
	} else if ( sMarkString.Find( _T( "?" ) ) == -1   && sMarkString.Find( _T( "*" ) ) == -1 )
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
#ifndef INCLUDE_BOOST_REGEX_AND_RELATED_LOGIC
	if ( IsBoostOrStd_Regex( m_Regex_Compalibility ) )
	{
		m_Regex_Compalibility = OldRegex_Compalibility;
		m_RegexCompatibility_cmbx.SetCurSel( (int)m_Regex_Compalibility );
		AfxMessageBox( _T( "Option not available on 32 bit platform" ) );
	}
#endif
	if ( IsBoostOrStd_Regex( OldRegex_Compalibility ) && !IsBoostOrStd_Regex( m_Regex_Compalibility ) )
	{
		CString sMarkString;
		m_RegexEditBox.GetWindowText( sMarkString );
		if ( sMarkString.Left( 4 ) == _T( "(?i)" ) )
		{
			m_RegexEditBox.SetWindowText( sMarkString.Mid( 4 ) );
			m_RegexEditBox.SetFocus();
			m_RegexEditBox.SetSel( 0, 0 );
		}
	}
	if ( m_EnableRegexCompatibilityOption )
	{
		if ( IsBoostOrStd_Regex( m_Regex_Compalibility ) )
		{
			m_ReplaceWithBox.EnableWindow( TRUE );
			m_ReplaceButton.EnableWindow( TRUE );
			m_ReplaceWithStaticText.EnableWindow( TRUE );
			if ( m_UndoSource.size() > 0 )
				m_ReplaceUndoButton.EnableWindow( TRUE );
		} else
		{
			m_ReplaceWithBox.EnableWindow( FALSE );
			m_ReplaceButton.EnableWindow( FALSE );
			m_ReplaceWithStaticText.EnableWindow( FALSE );
			m_ReplaceUndoButton.EnableWindow( FALSE );
		}
	}
	OnBnClickedIgnoreCaseCheck();

	OldRegex_Compalibility = m_Regex_Compalibility;
}

BOOL CRegexAssistantDlg::OnNotify( WPARAM wParam, LPARAM lParam, LRESULT* pResult )
{
	if ( ((LPNMHDR)lParam)->hwndFrom == m_ScintillaWrapper.GetWnd() )
	{
		SCNotification *pscn = (SCNotification *)lParam;
		*pResult = 0;
		if ( pscn->nmhdr.code == SCN_MODIFIED /*SCN_PAINTED*/ )
		{
			OnEnChangeRegexEditBox();
			if ( !m_bMakingChangeByReplacementLogic && m_UndoSource.size() )
			{
				m_UndoSource.clear();
				m_ReplaceUndoButton.EnableWindow( FALSE );
			}
		}
		return TRUE;
	}
	return CWnd::OnNotify( wParam, lParam, pResult );
}

void CRegexAssistantDlg::RegexReplace_BodyMethod( CString NeedleCstr, CString NeedleReplacementCstr )
{
	int BufferSize = m_ScintillaWrapper.SendEditor( SCI_GETTEXTLENGTH ) + 1;
	if ( BufferSize > 0 )
	{
		char* pcBuffer = new char[BufferSize + 1];
		m_ScintillaWrapper.SendEditor( SCI_GETTEXT, (uptr_t)BufferSize, reinterpret_cast<sptr_t>(pcBuffer) );
		pcBuffer[BufferSize] = '\0';
#ifdef INCLUDE_BOOST_REGEX_AND_RELATED_LOGIC
		try
		{
			string strHaystack = pcBuffer;
			string NeedleReplacement = FXString::ToString( (LPCTSTR)NeedleReplacementCstr );
			string strHaystackWithNeedleReplacement = regex_replace( strHaystack, GetNeedle( FXString::ToString( (LPCTSTR)NeedleCstr ).c_str() ), NeedleReplacement );
			if ( strHaystackWithNeedleReplacement != strHaystack )
			{
				m_UndoSource.push_back( strHaystack );
				m_ReplaceUndoButton.EnableWindow( TRUE );
			}
			m_ScintillaWrapper.SendEditor( SCI_CLEARALL );
			m_ScintillaWrapper.SendEditor( SCI_APPENDTEXT, strHaystackWithNeedleReplacement.length(), reinterpret_cast<sptr_t>(strHaystackWithNeedleReplacement.c_str()) );
		} catch ( ... )
		{
			//return;
		}
#endif
		delete[] pcBuffer;
	}
}

void CRegexAssistantDlg::OnBnClickedReplaceButton()
{
	if ( !m_ScintillaWrapper.IsInit() )
		return;
	CString NeedleCstr;
	m_RegexEditBox.GetWindowText( NeedleCstr );
	CString NeedleReplacementCstr;
	m_ReplaceWithBox.GetWindowText( NeedleReplacementCstr );
	m_bMakingChangeByReplacementLogic = true;
	if ( m_Regex_Compalibility == REGEX_COMPATIBILITY_BOOST_MULTILINE )
		RegexReplace_BodyMethod( NeedleCstr, NeedleReplacementCstr );
	else
		RegexReplace_LineByLine( NeedleCstr, NeedleReplacementCstr );
	m_bMakingChangeByReplacementLogic = false;
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
		m_ReplaceUndoButton.EnableWindow( FALSE );
}

#include <regex>
void CRegexAssistantDlg::RegexReplace_LineByLine( CString NeedleCstr, CString NeedleReplacementCstr )
{
	size_t BufferSize = m_ScintillaWrapper.SendEditor( SCI_GETTEXTLENGTH ) + 1;
	if ( BufferSize < 1 )
		return;
	char* pcBuffer = new char[BufferSize + 1];
	m_ScintillaWrapper.SendEditor( SCI_GETTEXT, (uptr_t)BufferSize, reinterpret_cast<sptr_t>(pcBuffer) );
	pcBuffer[BufferSize] = '\0';
	string strOriginalHaystack = pcBuffer;
	delete[] pcBuffer;
	pcBuffer = new char[m_MaxViewWidth + 10];
	string strFullHaystackReplacement;
#ifdef INCLUDE_BOOST_REGEX_AND_RELATED_LOGIC
	string NeedleReplacement = FXString::ToString( (LPCTSTR)NeedleReplacementCstr );
	sregex envar;
	std::regex std_envar;
	try
	{
		if ( IsBoostRegex( m_Regex_Compalibility ) )
			envar = GetNeedle( FXString::ToString( (LPCTSTR)NeedleCstr ).c_str() );
		else
		{
			string s = FXString::ToString( (LPCTSTR)NeedleCstr ).c_str();
			std_envar = std::regex( s );
		}
	} catch ( ... )
	{
		MessageBox( _T( "Error while trying to compile replacement string.  Check " ) );
	}
	for ( int LineCurrent = m_ScintillaWrapper.SendEditor( SCI_LINEFROMPOSITION, 0, 0 );; ++LineCurrent )
	{
		int LineLen = m_ScintillaWrapper.SendEditor( SCI_GETLINE, LineCurrent, reinterpret_cast<sptr_t>(pcBuffer) );
		if ( LineLen <= 0 )
			break;
		pcBuffer[LineLen] = '\0';
		try
		{
			string strHaystack = pcBuffer;
			string ConvertedString;
			try
			{
				if ( m_Regex_Compalibility == REGEX_COMPATIBILITY_STD_REGEX )				{
					ConvertedString = std::regex_replace( strHaystack, std_envar, NeedleReplacement );
				} else if ( m_Regex_Compalibility == REGEX_COMPATIBILITY_STD_REGEX_SED )
				{
					ConvertedString = std::regex_replace( strHaystack, std_envar, NeedleReplacement, std::regex_constants::format_sed );
				} else
				{
					boost::xpressive::regex_constants::match_flag_type frmt = boost::xpressive::regex_constants::format_all;
					if ( m_Regex_Compalibility == REGEX_COMPATIBILITY_BOOST_PERL )
						frmt = boost::xpressive::regex_constants::format_perl;
					else if ( m_Regex_Compalibility == REGEX_COMPATIBILITY_BOOST_SED )
						frmt = boost::xpressive::regex_constants::format_sed;
					else if ( m_Regex_Compalibility == REGEX_COMPATIBILITY_BOOST_DEFAULT )
						frmt = boost::xpressive::regex_constants::format_default;

					ConvertedString = regex_replace( strHaystack, envar, NeedleReplacement, frmt );
				}
			} catch ( ... )	{
				ConvertedString = strHaystack;
			}
			strFullHaystackReplacement += ConvertedString;
		} catch ( ... )	{
			break;
		}
	}
#endif
	if ( strOriginalHaystack != strFullHaystackReplacement )
	{
		m_UndoSource.push_back( strOriginalHaystack );
		m_ReplaceUndoButton.EnableWindow( TRUE );
		m_ScintillaWrapper.SendEditor( SCI_CLEARALL );
		m_ScintillaWrapper.SendEditor( SCI_APPENDTEXT, strFullHaystackReplacement.size(), reinterpret_cast<sptr_t>(strFullHaystackReplacement.c_str()) );
	}
	delete[] pcBuffer;
}

void CRegexAssistantDlg::OnBnClickedStaticGroupboxTestTargetText()
{
	CClipboardXX clipboard;
	std::string paste_text;
	clipboard >> paste_text;
	m_ScintillaWrapper.SendEditor( SCI_SETTEXT, 0, reinterpret_cast<sptr_t>(paste_text.c_str()) );
}

bool CRegexAssistantDlg::IsBoostRegex( Regex_Compatibility regex_compalibility )
{
	if ( regex_compalibility == REGEX_COMPATIBILITY_BOOST ||
		 regex_compalibility == REGEX_COMPATIBILITY_BOOST_MULTILINE ||
		 regex_compalibility == REGEX_COMPATIBILITY_BOOST_PERL ||
		 regex_compalibility == REGEX_COMPATIBILITY_BOOST_SED ||
		 regex_compalibility == REGEX_COMPATIBILITY_BOOST_DEFAULT
		 )
		return true;
	return false;
}

bool CRegexAssistantDlg::IsBoostOrStd_Regex( Regex_Compatibility regex_compalibility )
{
	if ( regex_compalibility == REGEX_COMPATIBILITY_STD_REGEX || regex_compalibility == REGEX_COMPATIBILITY_STD_REGEX_SED )
		return true;
	return IsBoostRegex( regex_compalibility );
}

bool CRegexAssistantDlg::IsScintillaRegex( Regex_Compatibility regex_compalibility )
{
	if ( regex_compalibility == REGEX_COMPATIBILITY_SCINTILLA_POSIX ||
		 regex_compalibility == REGEX_COMPATIBILITY_SCINTILLA )
		return true;
	return false;
}
