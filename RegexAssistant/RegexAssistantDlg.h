
// RegexAssistantDlg.h : header file
#pragma once
#include <vector>
#include <string>
#include "SizingDialog.h"
#include "ScintillaWrapper.h"

#define REPORT_ERR_AND_EXIT(exit_err_no, msg_format, ...) {CString msg; msg.Format(_T("Error:  " msg_format "  Performming early exit due to error."),__VA_ARGS__);AfxMessageBox(msg);exit(exit_err_no);}
class CRegexAssistantDlgAutoProxy;

enum Regex_Compatibility {
	REGEX_COMPATIBILITY_BOOST,
	REGEX_COMPATIBILITY_SCINTILLA_POSIX,
	REGEX_COMPATIBILITY_SCINTILLA,
	REGEX_COMPATIBILITY_BOOST_MULTILINE,
	REGEX_COMPATIBILITY_BOOST_PERL,
	REGEX_COMPATIBILITY_BOOST_SED,
	REGEX_COMPATIBILITY_BOOST_DEFAULT,
	REGEX_COMPATIBILITY_STD_REGEX,
	REGEX_COMPATIBILITY_STD_REGEX_SED,
	NUM_OF_MARKERS = 5
};

struct MARKERDATAtag
{
	COLORREF	TextColor;
	COLORREF	BkColor;
	BOOL		MarkLine;
};

class CRegexAssistantDlg :	public CSizingDialog 
{
	DECLARE_DYNAMIC(CRegexAssistantDlg);
	friend class CRegexAssistantDlgAutoProxy;
	enum { IdxRegex = 0, IdxDescription = 1, IdxExample = 2, IdxMatch = 3 };
	static const int QtyColumnsInLinst;
	static const int MaxInsertItemsList;
	static const CString InsertItemsList[];
public:
	CRegexAssistantDlg(CString regex_search, CString regex_replace, CString Sample, CRegexAssistantApp::SampleLoadMethod sampleloadmethod, int MonitorToDisplay, CWnd* pParent = nullptr);	// standard constructor
	virtual ~CRegexAssistantDlg();
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_REGEXASSISTANT_DIALOG };
#endif

protected:
	virtual BOOL OnNotify( WPARAM wParam, LPARAM lParam, LRESULT* pResult );
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	virtual BOOL OnInitDialog();
	void OnEnChangeRegexEditBox( CString MarkString, int FieldId, bool ClearMarkers, DWORD Flag = 0, int TargetLineNo = -1 );
	void OnEnChangeRegexEditBox_LineByLine( CString MarkString );
	void OnEnChangeRegexEditBox_BodyMethod( CString MarkString );
	void UpdateWindowTitle();
	void UpdateConversionButtonEnableStatus( CString MarkString );
	void RegexReplace_BodyMethod( CString MarkString, CString NeedleReplacementCstr );
	void RegexReplace_LineByLine( CString MarkString, CString NeedleReplacementCstr );
	static bool IsBoostRegex( Regex_Compatibility regex_compalibility );
	static bool IsBoostOrStd_Regex( Regex_Compatibility regex_compalibility );
	static bool IsScintillaRegex( Regex_Compatibility regex_compalibility );

	CString m_CurrentText;
	int m_FieldId;
	BOOL m_bCase;
	BOOL m_EnableRegexCompatibilityOption;
	Regex_Compatibility m_Regex_Compalibility;
	ScintillaWrapper m_ScintillaWrapper;
	int m_MaxViewWidth;
	std::vector<CString> m_RegexList;
	static MARKERDATAtag m_MarkerData[NUM_OF_MARKERS];
	std::vector<std::string> m_UndoSource;
	bool m_bMakingChangeByReplacementLogic;
	CRegexAssistantDlgAutoProxy* m_pAutoProxy;
	HICON m_hIcon;
	BOOL CanExit();
	CEdit m_RegexEditBox;
	CListCtrl m_TokenListCtrl;
	CButton m_Case;
	CString m_TestTargetTextData;
	static const CString m_DefaultTestTargetTextData;
	CComboBox m_RegexCompatibility_cmbx;
	CButton m_ConvertSqlWildToRegex_btn;
	CButton m_ConvertFilesysWildToRegex_btn;
	CEdit m_ReplaceWithBox;
	CButton m_ReplaceButton;
	CStatic m_ReplaceWithStaticText;
	CButton m_ReplaceUndoButton;
	CStatic m_GroupBoxToken;
	DWORD lastErr;
	int m_MonitorToDisplay;
	CRegexAssistantApp::SampleLoadMethod m_SampleLoadMethod;

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnClose();
	afx_msg void OnBnClickedOk();
	afx_msg void OnNMDblclkTokenListCtrl( NMHDR *pNMHDR, LRESULT *pResult );
	afx_msg void OnBnClickedIgnoreCaseCheck();
	afx_msg void OnBnClickedConvertSqlWildToRegexButton();
	afx_msg void OnBnClickedConvertFilesystemWildToRegexButton();
	afx_msg void OnEnChangeRegexEditBox();
	afx_msg void OnCbnSelchangeRegexCompatibilitySelectionCombo();
	afx_msg void OnBnClickedReplaceButton();
	afx_msg void OnBnClickedReplaceUndoButton();
	afx_msg void OnBnClickedStaticGroupboxTestTargetText();
	inline virtual void OnOK() { OnClose(); };
	inline virtual void OnCancel() { OnClose(); };
};
