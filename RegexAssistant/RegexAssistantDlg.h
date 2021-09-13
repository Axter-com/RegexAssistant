/*
	Copyright (C) 2021  David Maisonave
	The RegexAssistant source code is free software. You can redistribute it and/or modify it under the terms of the GNU General Public License.
	This program is distributed in the hope that it will be useful,	but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
*/
#pragma once
#include "RegexAssistant.h"
#include <vector>
#include <map>
#include <string>
#include "ScintillaWrapper.h"
#include "mfcx.ComboBox.h"
#include <boost/xpressive/xpressive.hpp>

#define REPORT_ERR_AND_EXIT(exit_err_no, msg_format, ...) {CString msg; msg.Format(_T("Error:  " msg_format "  Performming early exit due to error."),__VA_ARGS__);AfxMessageBox(msg);exit(exit_err_no);}
class CRegexAssistantDlgAutoProxy;
struct MARKERDATAtag
{
	COLORREF	TextColor;
	COLORREF	BkColor;
	BOOL		MarkLine;
};

struct RegexCompatibilityProperties
{
	CString Name; // Name shown on the GUI dropdown list
	enum Regex_Compatibility regex_compatibility_ID; //ID unique to this option
	int regex_compatibility_basecode; //Base implementation
	BOOL IsItemEnabled; // Set true if fully implemented or compiling in debug mode.
	std::string CommandLineOption; // string used on command line to select this compatibility
	std::string HelpTip; // Summary of type
	int HighLightIndexID;
};

class CRegexAssistantDlg : public CDialog
{
	DECLARE_DYNAMIC( CRegexAssistantDlg );
	friend class CRegexAssistantDlgAutoProxy;
	enum { IdxRegex = 0, IdxDescription = 1, IdxExample = 2, IdxMatch = 3 };
	// Private static member variables
	static const int QtyColumnsInLinst;
	static const int MaxInsertItemsList;
	static const CString InsertItemsList[];
	static const CString m_CompatibilityComboBoxSelections[];
	static const CString m_DefaultTestTargetTextData;
	static const MARKERDATAtag m_MarkerData[];
	static const RegexCompatibilityProperties m_RegexCompatibilityProperties[];
public:
	CRegexAssistantDlg( CString regex_search, CString regex_replace, CString Sample, CRegexAssistantApp::SampleLoadMethod sampleloadmethod,
						int MonitorToDisplay, Regex_Compatibility regex_compatibility, CWnd* pParent = nullptr );
	virtual ~CRegexAssistantDlg();
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_REGEXASSISTANT_DIALOG };
#endif
protected:
	// Functions for internal usage
	void OnEnChangeRegexEditBox( CString MarkString, DWORD Flag = 0 );
	void OnEnChangeRegexEditBox_LineByLine( CString MarkString );
	void OnEnChangeRegexEditBox_BodyMethod( CString MarkString );
	void UpdateWindowTitle();
	void UpdateConversionButtonEnableStatus( CString MarkString );
	void RegexReplace_Scintilla( CString MarkString, CString NeedleReplacementCstr );
	void RegexReplace_BodyMethod( CString MarkString, CString NeedleReplacementCstr );
	void RegexReplace_LineByLine( CString MarkString, CString NeedleReplacementCstr );
	void FetchTextForUndoArray();
	void PopulateTokenList();
	void CreateSizeGrip();
	BOOL CanExit();
	int GetMarkerID();
	bool IsStd_Regex();
	bool IsBoostRegex();
	bool IsBoostDefaultRegex();
	bool IsBoostAllRegex();
	bool IsBoostOrStd_Regex();
	bool IsScintillaRegex();
	bool IsScintillaStandardRegex();
	bool IsStd_Regex( Regex_Compatibility regex_compalibility );
	bool IsBoostRegex( Regex_Compatibility regex_compalibility );
	bool IsBoostOrStd_Regex( Regex_Compatibility regex_compalibility );
	bool IsScintillaRegex( Regex_Compatibility regex_compalibility );
	bool IsNotCompatibleWithBackSlashReplacementToken();
	bool IsNotCompatibleWithDollarSignReplacementToken();
	bool IsPOSIX();
	bool IsSED();
	bool IsPerl();
	bool IsMultiline();
	bool IsUNIX_OLD_SYNTAX();
	boost::xpressive::regex_constants::match_flag_type GetBoostCompatibilityFlag( Regex_Compatibility regex_compatibility );
	virtual BOOL OnNotify( WPARAM wParam, LPARAM lParam, LRESULT* pResult );
	virtual void DoDataExchange( CDataExchange* pDX );	// DDX/DDV support
	virtual BOOL OnInitDialog();

	// Member non-static Variables
	wchar_t *m_py_decodelocale;
	std::string m_OriginalSampleValue;
	std::vector<CString> m_RegexList;
	std::vector<std::string> m_UndoSource;
	CString m_CurrentRegexStatement;
	CString m_SampleText;
	int m_MaxViewWidth;
	int m_MonitorToDisplay;
	BOOL m_bCase;
	bool m_bMakingChangeByReplacementLogic;
	DWORD m_dwLastErr;
	Regex_Compatibility m_Regex_Compalibility;
	ScintillaWrapper m_ScintillaWrapper;
	CRegexAssistantDlgAutoProxy* m_pAutoProxy;
	CRegexAssistantApp::SampleLoadMethod m_SampleLoadMethod;
	HWND			m_hSizeGrip;			// handle of the size grip
	HICON m_hIcon;
	// Controls
	CEdit m_RegexStatement_editBx;
	CListCtrl m_TokenList_list;
	mfcx::ComboBox m_RegexCompatibility_cmbx;
	CEdit m_RegexReplacementExpression_editBx;
	CStatic m_RegexReplacementExpression_Label_static;
	CStatic m_TokenList_Label_static;
	CButton m_Case_btn;
	CButton m_RunRegexReplacement_btn;
	CButton m_UndoRegexReplacementChanges_btn;
	CButton m_ConvertSqlWildToRegex_btn;
	CButton m_ConvertFilesysWildToRegex_btn;
	CButton m_ResetSampleContent_btn;
	CButton m_UndoExpressionChange_btn;
public:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnSysCommand( UINT nID, LPARAM lParam );
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
	afx_msg void OnBnClickedResetSample();
	afx_msg void OnBnClickedUndoExpressionChange();
	inline virtual void OnOK() { OnClose(); };
	inline virtual void OnCancel() { OnClose(); };
	CStatic m_SampleText_Label_static;
};
