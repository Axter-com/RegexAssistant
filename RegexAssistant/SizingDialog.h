//////////////////////////////////////////////////////////////////////////
// 
// CSizingDialog V2.1
// 
// Copyright 2005 Xia Xiongjun( ÏÄÐÛ¾ü ), All Rights Reserved.
//
// E-mail: xj-14@163.com
//
// This source file can be copyed, modified, redistributed  by any means
// PROVIDING that this notice and the author's name and all copyright 
// notices remain intact, and PROVIDING it is NOT sold for profit without 
// the author's expressed written consent. The author accepts no 
// liability for any damage/loss of business that this product may cause.
//
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// SizingDialog.h 

#pragma once

#include "SizingSP.h"


//////////////////////////////////////////////////////////////////////////
// CSizingDialog dialog

class CSizingDialog : public CDialogEx
{
	DECLARE_SIZING_SUPPORT()

// Constructors
public:
	enum CSIZINGDIALOG_RWP_OPTION {RWP_SESSION, RWP_PERSISTENT, RWP_DISABLED};
	CSizingDialog(BOOL	bEnableRWP = TRUE, LPCTSTR lpszSection = NULL, LPCTSTR lpszEntry = NULL);
	explicit CSizingDialog(UINT nIDTemplate, CWnd* pParentWnd = NULL, CSIZINGDIALOG_RWP_OPTION	csizingdialog_rwp_option = RWP_SESSION, 
		LPCTSTR lpszSection = _T("Placement"), LPCTSTR lpszEntry = _T("MainDialog"));

	void SetOrgSizeAsMinSize();
// Implementation
protected:
	afx_msg void OnPaint();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnDestroy();
	afx_msg LRESULT OnNcHitTest(CPoint point);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI);
	DECLARE_MESSAGE_MAP()

// Data
private:
	LPCTSTR			m_lpszSection;
	LPCTSTR			m_lpszEntry;
	BOOL			m_bEnableRWP;		// Enable to restore the window placement
	BOOL			m_bErase;			// Enable erase background
	const CSIZINGDIALOG_RWP_OPTION	m_csizingdialog_rwp_option;
	static WINDOWPLACEMENT m_pwp_session;
	static BOOL				m_bPwpFetched;
	int m_Min_cy;
	int m_Min_cx;
};

class CSizingFormView : public CFormView
{
};

