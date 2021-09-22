/*
	Copyright (C) 2021  David Maisonave
	The RegexAssistant source code is free software. You can redistribute it and/or modify it under the terms of the GNU General Public License.
	This program is distributed in the hope that it will be useful,	but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
*/
#pragma once
#include "RegexAssistantDlg_enums.h"
#include <vector>
#include <map>
#include <string>
#include "ScintillaWrapper.h"
#include "mfcx.ComboBox.h"
#include <boost/xpressive/xpressive.hpp>
#include <regex>

class CRegexAssistantDlgAutoProxy;
struct MARKERDATAtag
{
	COLORREF	TextColor;
	COLORREF	BkColor;
	BOOL		MarkLine;
};

struct RegexCompatibilityProperties
{
	CString Name;								// Name shown on the dropdown list
	enum Regex_Compatibility CompatibilityID;	// ID unique to this option
	int CompatibilityAttributes;				// Base implementation
	BOOL IsItemEnabled;							// Set true if fully implemented or compiling in debug mode.
	std::string CommandLineOption;				// string used on command line to select this compatibility
	std::string HelpTip;						// Summary of type
	int HighLightIndexID;						// Color index to mark text in the sample window
};

class CEditGetKeys : public CEdit
{
public:
	CEditGetKeys() {};

	//{{AFX_MSG(CEditGetKeys)
	afx_msg void OnKeyDown( UINT nChar, UINT nRepCnt, UINT nFlags );
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

class CMFCListCtrl_AltRowColors : public CMFCListCtrl
{
public:
	CMFCListCtrl_AltRowColors() {};
	virtual COLORREF OnGetCellBkColor(int nRow, int nColumn );
};

struct SessionSettings
{
	CString Expression;
	CString ReplacementExpression;
	CString SampleText;
};


class CRegexAssistantDlg : public CDialog
{
	DECLARE_DYNAMIC( CRegexAssistantDlg );
	friend class CRegexAssistantDlgAutoProxy;
	enum { IdxRegex = 0, IdxDescription = 1, IdxExample = 2, IdxSample = 3, IdxMatch = 4, MaxUndoRegex = 250, MaxUndoSample = 64 };
	// Private static member variables
	static const int m_MaxViewWidth;
	static const int m_ViewWidthAddedPad;
	static const int QtyColumnsInLinst;
	static const int MaxInsertItemsList;
	static const CString InsertItemsList[];
	static const CString m_CompatibilityComboBoxSelections[];
	static const CString m_DefaultTestTargetTextData;
	static const MARKERDATAtag m_MarkerData[];
	static const RegexCompatibilityProperties m_RegexCompatibilityProperties[];
public:
	CRegexAssistantDlg( CString regex_search, CString regex_replace, CString Sample, SampleLoadMethod sampleloadmethod,
						int MonitorToDisplay, Regex_Compatibility regex_compatibility, CWnd* pParent = nullptr );
	virtual ~CRegexAssistantDlg();
	void KeyUpOrDown( UINT nChar );
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_REGEXASSISTANT_DIALOG };
#endif
protected:
	// Functions for internal usage
	int GetMaxUndoSampleChange( int SampleSize );
	CString CopyRegexStatementForUndo();
	void UpdateWindowTitle();
	void UpdateConversionButtonEnableStatus( CString MarkString );
	void ChangeRegexEditBox_Scintilla( CString MarkString, DWORD Flag = 0 );
	void ChangeRegexEditBox_BodyMethod( CString MarkString );
	void RegexReplace_Scintilla( CString MarkString, CString NeedleReplacementCstr );
	void RegexReplace_BodyMethod( CString MarkString, CString NeedleReplacementCstr );
	//void RegexReplace_LineByLine( CString MarkString, CString NeedleReplacementCstr );
	//void ChangeRegexEditBox_LineByLine( CString MarkString );
	void FetchTextForUndoArray();
	void PopulateTokenList();
	void CreateSizeGrip();
	void DragAndDrop(CString Name);
	void UpdateIgnoreCaseStatus(BOOL UpdateButtonStatus = FALSE, BOOL UpdateExpression = FALSE);
	BOOL CanExit();
	int GetMarkerID();
	bool IsSTD_Regex();
	bool IsBoost();
	bool IsBoostDefault();
	bool IsBoostAll();
	bool IsBoostOrSTD();
	bool IsScintilla();
	bool IsScintillaStandard();
	bool IsSTD_Regex( Regex_Compatibility regex_compalibility );
	bool IsBoost( Regex_Compatibility regex_compalibility );
	bool IsBoostOrSTD( Regex_Compatibility regex_compalibility );
	bool IsScintilla( Regex_Compatibility regex_compalibility );
	bool IsNotCompatibleWithBackSlashReplacementToken();
	bool IsNotCompatibleWithDollarSignReplacementToken();
	bool IsPOSIX();
	bool IsSED();
	bool IsPerl();
	bool IsMultiline();
	bool IsUNIX_OLD_SYNTAX();
	boost::xpressive::regex_constants::match_flag_type GetBoostCompatibilityFlag();
	std::regex_constants::syntax_option_type GetStdRegexOptionFlags();
	std::regex_constants::match_flag_type GetStdRegexMatchFlags();
	virtual BOOL OnNotify( WPARAM wParam, LPARAM lParam, LRESULT* pResult );
	virtual void DoDataExchange( CDataExchange* pDX );	// DDX/DDV support
	virtual BOOL OnInitDialog();

	// Member non-static Variables
	SessionSettings m_SessionSettings;
	WINDOWPLACEMENT *m_OriginalPlacement;
	wchar_t *m_py_decodelocale;
	std::string m_OriginalSampleValue;
	std::vector<CString> m_RegexStatementChangesToUndo;
	std::vector<std::string> m_SampleChangesToUndo;
	const CString m_OriginalRegexStatement;
	CString m_LastRegexStatement;
	CString m_SampleText;
	int m_MonitorToDisplay;
	BOOL m_IgnoreCase;
	bool m_bMakingChangeByReplacementLogic;
	DWORD m_dwLastErr;
	Regex_Compatibility m_Regex_Compalibility;
	ScintillaWrapper m_ScintillaWrapper;
	CRegexAssistantDlgAutoProxy* m_pAutoProxy;
	SampleLoadMethod m_SampleLoadMethod;
	HWND m_hSizeGrip; // handle of the size grip at the bottom right corner of the main window
	HICON m_hIcon;
	// Controls
	mfcx::ComboBox m_RegexCompatibility_cmbx;
	CEditGetKeys m_RegexStatement_editBx;
	CMFCListCtrl_AltRowColors  m_TokenList_list;
	CEditGetKeys m_RegexReplacementExpression_editBx;
	CStatic m_RegexReplacementExpression_Label_static;
	CStatic m_TokenList_Label_static;
	CStatic m_SampleText_Label_static;
	CButton m_Case_btn;
	CButton m_RunRegexReplacement_btn;
	CButton m_UndoRegexReplacementChanges_btn;
	CButton m_ConvertSqlWildToRegex_btn;
	CButton m_ConvertFilesysWildToRegex_btn;
	CButton m_ResetSampleContent_btn;
	CButton m_UndoExpressionChange_btn;
protected:
	afx_msg void OnSysCommand( UINT nID, LPARAM lParam );
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnClose();
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
	inline virtual void OnOK() {};
	inline virtual void OnCancel() { OnClose(); };
	afx_msg void OnDropFiles( HDROP hDropInfo );
	afx_msg BOOL OnQueryOpen();
	afx_msg void OnNMDblclkTokenListCtrl( NMHDR *pNMHDR, LRESULT *pResult );
	DECLARE_MESSAGE_MAP()
};
