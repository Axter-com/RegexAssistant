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
// SizingDialog.cpp :  implementation file

#include "pch.h"
#include "SizingDialog.h"
#include ".\SizingDialog.h"

#define ID_TIMER				1


//////////////////////////////////////////////////////////////////////////
// CSizingDialog dialog

IMPLEMENT_SIZING_SUPPORT(CSizingDialog)

CSizingDialog::CSizingDialog(BOOL	bEnableRWP,
	LPCTSTR lpszSection /*= NULL*/, LPCTSTR lpszEntry /*= NULL*/)
	: m_lpszSection(lpszSection)
	, m_lpszEntry(lpszEntry)
	, m_bEnableRWP(bEnableRWP)
	, m_bErase(TRUE), m_Min_cy(0), m_Min_cx(0)
	, m_csizingdialog_rwp_option(RWP_PERSISTENT)
{
	CDialogEx::CDialogEx();
}

CSizingDialog::CSizingDialog(
	UINT nIDTemplate,
	CWnd* pParentWnd /*= NULL*/, CSIZINGDIALOG_RWP_OPTION	csizingdialog_rwp_option, 
	LPCTSTR lpszSection /*= _T("Placement")*/, 
	LPCTSTR lpszEntry /*= _T("MainDialog")*/)
	: CDialogEx(nIDTemplate, pParentWnd)
	, m_lpszSection(lpszSection)
	, m_lpszEntry(lpszEntry)
	, m_bEnableRWP(TRUE)
	, m_bErase(TRUE), m_Min_cy(0), m_Min_cx(0)
	, m_csizingdialog_rwp_option(csizingdialog_rwp_option)
{
	if (m_csizingdialog_rwp_option ==	RWP_DISABLED)
		m_bEnableRWP = FALSE;
}

BEGIN_MESSAGE_MAP(CSizingDialog, CDialogEx )
	ON_WM_CREATE()
	ON_WM_SHOWWINDOW()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_WM_TIMER()
	ON_WM_DESTROY()
	ON_WM_NCHITTEST()
	ON_WM_GETMINMAXINFO()
END_MESSAGE_MAP()

void CSizingDialog::SetOrgSizeAsMinSize()
{
	if (m_bEnableRWP == FALSE)
		return;

	if (m_Min_cy != 0 && m_Min_cx != 0)
		return;

	CRect rect;
	GetClientRect(&rect);
	m_Min_cx = rect.Width();
	m_Min_cy = rect.Height();
}

// CSizingDialog message handlers

int CSizingDialog::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if ( CDialogEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	::SetWindowLong(m_hWnd, GWL_STYLE, 
		::GetWindowLong(m_hWnd, GWL_STYLE) | WS_THICKFRAME);

	Initialize(m_hWnd);

	return 0;
}

WINDOWPLACEMENT CSizingDialog::m_pwp_session = {0};
BOOL CSizingDialog::m_bPwpFetched = FALSE;

void CSizingDialog::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogEx::OnShowWindow(bShow, nStatus);

	if (m_bEnableRWP == FALSE)
		return;

	// The following code will be run only once
	m_bEnableRWP = FALSE;

	if (m_csizingdialog_rwp_option == RWP_PERSISTENT)
	{
		// Restore window placement
		if (m_lpszSection == NULL || m_lpszEntry == NULL)
			return;

		WINDOWPLACEMENT* pwp;
		UINT nBytes;
		if (::AfxGetApp()->GetProfileBinary(m_lpszSection, m_lpszEntry, 
			(LPBYTE *)&pwp, &nBytes))
		{
			if (pwp->showCmd == SW_SHOWMINIMIZED)
				pwp->showCmd = pwp->flags + 1;
			if (pwp->showCmd == SW_SHOWMAXIMIZED)
				m_bErase = FALSE;
			SetWindowPlacement(pwp);

			MYDELETEARRAY(pwp);
		}
	}
	else if (m_csizingdialog_rwp_option == RWP_SESSION)
	{
		if (m_bPwpFetched)
		{
			SetWindowPlacement(&m_pwp_session);
		}
	}
}

void CSizingDialog::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	m_objSizingSP.ResizeControls();
	if (IsWindowVisible())
	{
		::SetTimer(m_hWnd, ID_TIMER, 10, NULL);
		//InvalidateRect(NULL, TRUE);
	}
}

void CSizingDialog::OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI)
{
  // set the minimum tracking width
  // and the minimum tracking height of the window
  lpMMI->ptMinTrackSize.x = m_Min_cx;
  lpMMI->ptMinTrackSize.y = m_Min_cy;
}

void CSizingDialog::OnTimer(UINT_PTR nIDEvent)
{
	::KillTimer(m_hWnd, ID_TIMER);
	m_objSizingSP.UpdateGBoxFPic();
	InvalidateRect(NULL, TRUE);

	CDialogEx::OnTimer(nIDEvent);
}

void CSizingDialog::OnPaint() 
{
	CDialogEx::OnPaint();
}

BOOL CSizingDialog::OnEraseBkgnd(CDC* pDC)
{
	//return CDialogEx::OnEraseBkgnd(pDC);

	if (m_bErase != TRUE)
	{
		m_bErase = TRUE;
		return FALSE;
	}

	m_objSizingSP.EraseBkgnd(pDC->m_hDC);
	return TRUE;
}

void CSizingDialog::OnDestroy()
{
	CDialogEx::OnDestroy();

	if (m_csizingdialog_rwp_option == RWP_PERSISTENT)
	{
		// Save window placement
		if (m_lpszSection == NULL || m_lpszEntry == NULL)
			return;

		WINDOWPLACEMENT wp;
		GetWindowPlacement(&wp);
		::AfxGetApp()->WriteProfileBinary(m_lpszSection, m_lpszEntry, 
			(LPBYTE)&wp, sizeof(wp));
	}
	else if (m_csizingdialog_rwp_option == RWP_SESSION)
	{
		m_bPwpFetched = TRUE;
		GetWindowPlacement(&m_pwp_session);
	}
}

LRESULT CSizingDialog::OnNcHitTest(CPoint point)
{
	// move the window when clicked
	LRESULT nHitTest = CDialogEx::OnNcHitTest(point);
	if(nHitTest == HTCLIENT)
		nHitTest = HTCAPTION;
	return nHitTest;
}

