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
// SizingSP.cpp :  implementation file

#include "pch.h"
#include "SizingSP.h"


#define CK_GROUPBOX				9
#define CK_FRAMEPICTRUE			5
#define CK_SPECIALCTRLS			0
#define CK_OTHERS				2


//////////////////////////////////////////////////////////////////////////
// class CListElement 

CListElement::CListElement(HWND hCtrl, RECT const& rcInit, double const* pdRate, 
						   CListElement* pNext)
						   : m_hCtrl(hCtrl)
						   , m_rcInit(rcInit)
						   , m_pNext(pNext)
{
	for (int i = 0; i < 4; i++)
		m_dRate[i] = pdRate[i];
}


//////////////////////////////////////////////////////////////////////////
// class CSizingSP 

CSizingSP::CSizingSP()
	: m_hParent(NULL)
	, m_pHead(NULL)
	, m_pTail(NULL)
	, m_hSizeGrip(NULL)
	, m_nCmdShowSizeGrip(SW_SHOWDEFAULT)
	, m_clrBackground(g_clrBtnFace)
	, m_hRgnErase(NULL)
	, m_hRgnCtrl(NULL)
{
	::SetRectEmpty(&m_rcInitClt);
}


void CSizingSP::Initialize(HWND hParent)
{
	ASSERT(hParent != NULL);
	m_hParent = hParent;

	// save initial window size to m_rcInitDlg
	::GetClientRect(m_hParent, &m_rcInitClt);
	
	// create the region-object for erase
	m_hRgnErase = ::CreateRectRgn(0, 0, 0, 0);

	// create the region-object for controls
	m_hRgnCtrl = ::CreateRectRgn(0, 0, 0, 0);
	
	// Create size grip
	CreateSizeGrip();
}


CSizingSP::~CSizingSP()
{
	while(m_pHead != NULL)
	{
		CListElement const* pTmp = m_pHead;
		m_pHead = m_pHead->m_pNext;
		MYDELETE(pTmp);
	}
	m_pTail = NULL;

	::DeleteObject(m_hRgnErase);
	::DeleteObject(m_hRgnCtrl);
}


//////////////////////////////////////////////////////////////////////////
// Add resizable controls

void CSizingSP::AddControl(HWND hCtrl, LPCVOID lpData, int nDataType)
{
	if ( hCtrl == NULL )
		return;
	ASSERT(::IsChild(m_hParent, hCtrl));

	if (nDataType == DATATYPE_NUMBER)
	{
		Append(hCtrl, (double const*)lpData);
		return;
	}

	double dRate[4] = {0.0, 0.0, 0.0, 0.0};
	GetRatesFromStr((LPCTSTR)lpData, hCtrl, dRate);
	Append(hCtrl, dRate);
}


void CSizingSP::AddControl(UINT nID, LPCVOID lpData, int nDataType)
{
	if (nID == NULL)		// all the controls
	{
		for (HWND hCtrl = ::GetWindow(m_hParent, GW_CHILD); hCtrl != NULL;
			hCtrl = ::GetNextWindow(hCtrl, GW_HWNDNEXT))
		{
			if (hCtrl == m_hSizeGrip)
				continue;

			// add all controls in turn
			UINT nIDCur = ::GetDlgCtrlID(hCtrl);
			AddControl(nIDCur, lpData, nDataType);
		}
		return;
	}

	HWND hCtrl = ::GetDlgItem(m_hParent, nID);
	AddControl(hCtrl, lpData, nDataType);

}


void CSizingSP::AddControlRange(UINT nIDS, UINT nIDE, LPCVOID lpData, int nDataType)
{
	StandardizeID(nIDS, nIDE);

	HWND hCtrlS = ::GetDlgItem(m_hParent, nIDS);
	HWND hCtrlE = ::GetDlgItem(m_hParent, nIDE);
	HWND hCtrlE_Next = ::GetNextWindow(hCtrlE, GW_HWNDNEXT);
	HWND hCtrlTmp = hCtrlS;
	do{
		UINT nIDTmp = ::GetDlgCtrlID(hCtrlTmp);
		AddControl(nIDTmp, lpData, nDataType);
		hCtrlTmp = ::GetNextWindow(hCtrlTmp, GW_HWNDNEXT);
	}while(hCtrlTmp != hCtrlE_Next);
}


void CSizingSP::AddControlArray(UINT nIDS, UINT nIDE, 
								double dC /*= 0.0*/, double dR /*= 1.0*/, BOOL bHori /*= TRUE*/,
								LPCTSTR lpszStrExtra /*= _T("Y")*/)
{
	StandardizeID(nIDS, nIDE);

	// calculate the moving parameters of the controls
	int n = GetCtrlCount(nIDS, nIDE);
	double dI = (1.0 - n*dC)/(n + 2*dR - 1);

	int i = 0;
	HWND hCtrlS = ::GetDlgItem(m_hParent, nIDS);
	HWND hCtrlE = ::GetDlgItem(m_hParent, nIDE);
	HWND hCtrlE_Next = ::GetNextWindow(hCtrlE, GW_HWNDNEXT);
	HWND hCtrlTmp = hCtrlS;
	do
	{
		double dRate[4] = {0.0, 0.0, 0.0, 0.0};
		GetRatesFromStr(lpszStrExtra, hCtrlTmp, dRate);

		double dP = dR*dI + dC*i + dI*i;
		if (bHori)
		{
			dRate[0] = dP;
			dRate[1] = dP + dC;
		}
		else
		{
			dRate[2] = dP;
			dRate[3] = dP + dC;
		}
		Append(hCtrlTmp, dRate);

		i++;
		hCtrlTmp = ::GetNextWindow(hCtrlTmp, GW_HWNDNEXT);
	}
	while(hCtrlTmp != hCtrlE_Next);
}


//////////////////////////////////////////////////////////////////////////
// Resize support

BOOL CSizingSP::ResizeControls() const
{
	if (m_pHead == NULL)
		return FALSE;

	// calculate increment of the dialog moving
	RECT rcClt;
	::GetClientRect(m_hParent, &rcClt);
	int dx = (rcClt.right - rcClt.left) - (m_rcInitClt.right - m_rcInitClt.left);
	int dy = (rcClt.bottom - rcClt.top) - (m_rcInitClt.bottom - m_rcInitClt.top);

	CListElement* pTmp = m_pHead;
	while (pTmp != NULL)
	{
		HWND    hCtrl  = pTmp->m_hCtrl;
		RECT&   rcInit = pTmp->m_rcInit;
		double* dRate  = pTmp->m_dRate;
		pTmp = pTmp->m_pNext;

		// Calculate the new position
		RECT rcCtrlOld;
		GetWindowRectClient(m_hParent, hCtrl, &rcCtrlOld);

		RECT rcCtrl = rcInit;
		InflateRect(&rcCtrl, int(-dx*dRate[0]), int(-dy*dRate[2]), 
			int(dx*dRate[1]),  int(dy*dRate[3]));

		if (IsRectEqual(&rcCtrl, &rcCtrlOld))
			continue;

		// Move the control
		BOOL bVisible = ::IsWindowVisible(hCtrl);
		if (bVisible)
			::SendMessage(hCtrl, WM_SETREDRAW, (WPARAM)FALSE, 0);

		::MoveWindow(hCtrl, rcCtrl.left, rcCtrl.top, 
			rcCtrl.right - rcCtrl.left, rcCtrl.bottom - rcCtrl.top, TRUE);

		BOOL bShow = 
			(hCtrl == m_hSizeGrip) ? AnalyseCmdShowSizeGrip() : bVisible;
		if (!bShow)
			continue;
		::SendMessage(hCtrl, WM_SETREDRAW, (WPARAM)TRUE, 0);

		// Invalidate the control region
		InvalidateCtrl(hCtrl, rcCtrlOld, rcCtrl);
	}
	return TRUE;
}


BOOL CSizingSP::EraseBkgnd(HDC hDC)
{
	ASSERT(hDC != NULL);

	// update SizeGrip
	if (m_hSizeGrip != NULL)
		ShowSizeGrip(m_nCmdShowSizeGrip);

	// create a brush to fill the background
	HBRUSH hBrush = ::CreateSolidBrush(m_clrBackground);

	// set the region-object for erase to the client eara of the window
	RECT rcClient;
	::GetClientRect(m_hParent, &rcClient);
	::SetRectRgn(m_hRgnErase, rcClient.left, rcClient.top,
		rcClient.right, rcClient.bottom);

	// cycle through all visible controls except group box and frame picture and
	// remove their area from hRgnErase
	HWND hCtrl = ::GetWindow(m_hParent, GW_CHILD);
	while (hCtrl != NULL)
	{
		if (::IsWindowVisible(hCtrl) 
			&& GetControlKind(hCtrl) < CK_FRAMEPICTRUE)	// FramePic and GBox is erased
		{
			RECT rcCtrl;
			GetWindowRectClient(m_hParent, hCtrl, &rcCtrl);
			::SetRectRgn(m_hRgnCtrl, rcCtrl.left, rcCtrl.top, 
				rcCtrl.right, rcCtrl.bottom);
			::CombineRgn(m_hRgnErase, m_hRgnErase, m_hRgnCtrl, RGN_DIFF);
		}
		hCtrl = ::GetWindow(hCtrl, GW_HWNDNEXT);
	}
	
	::FillRgn(hDC, m_hRgnErase, hBrush);

	::DeleteObject(hBrush);

	return TRUE;
}


void CSizingSP::UpdateGBoxFPic() const
{
	HWND hCtrl = ::GetWindow(m_hParent, GW_CHILD);
	while (hCtrl != NULL)
	{
		if (GetControlKind(hCtrl) >= CK_FRAMEPICTRUE)
		{
			RECT rc;
			GetWindowRectClient(m_hParent, hCtrl, &rc);
			::InvalidateRect(m_hParent, &rc, TRUE);
		}
		hCtrl = ::GetWindow(hCtrl, GW_HWNDNEXT);
	}
}


//////////////////////////////////////////////////////////////////////////
// Size grip for dialog

BOOL CSizingSP::ShowSizeGrip(int nCmdShow /*= SW_SHOWDEFAULT*/)
{
	ASSERT(m_hSizeGrip != NULL);

	int nCmdShowAnalysed = SW_SHOWDEFAULT;
	if (nCmdShow == SW_HIDE || nCmdShow == SW_SHOW)
	{
		m_nCmdShowSizeGrip = nCmdShow;
		nCmdShowAnalysed = nCmdShow;
	}
	else
	{
		m_nCmdShowSizeGrip = SW_SHOWDEFAULT;
		nCmdShowAnalysed = AnalyseCmdShowSizeGrip() ? SW_SHOW : SW_HIDE;
	}
	return ::ShowWindow(m_hSizeGrip, nCmdShowAnalysed);
}


//////////////////////////////////////////////////////////////////////////
// window background color support

COLORREF CSizingSP::GetBgColor() const
{
	return m_clrBackground;
}


void CSizingSP::SetBgColor(COLORREF clrColor)
{
	m_clrBackground = clrColor;
}


//////////////////////////////////////////////////////////////////////////
// redraw controls support

void CSizingSP::InvalidateCtrl(
	HWND hCtrl, RECT const& rcCtrlOld, RECT const& rcCtrl) const
{
	ASSERT(::IsChild(m_hParent, hCtrl));

	int nSize = GetControlKind(hCtrl);
	if (nSize == CK_SPECIALCTRLS)				// Need not to redraw
		return;

	LPRECT lprcOld, lprc;
	lprcOld	 = new RECT[nSize];
	lprc	 = new RECT[nSize];
	GetCtrlFragRects(hCtrl, &rcCtrlOld, lprcOld, nSize);
	GetCtrlFragRects(hCtrl, &rcCtrl, lprc, nSize);

	if (nSize == CK_GROUPBOX)
	{
		for (int i = 0; i < 9; i++)				// for xp style
			::InflateRect(&lprcOld[i], 1, 1);
	}

	for (int i = 1; i < nSize; i++)
		InvalidateCtrlBorder(&lprcOld[i], &lprc[i]);

	::InvalidateRect(hCtrl, NULL, FALSE);

	MYDELETEARRAY(lprcOld);
	MYDELETEARRAY(lprc);
}


// Erase lpRectOld - lpRectNew
void CSizingSP::InvalidateCtrlBorder(LPCRECT lpRectOld, LPCRECT lpRectNew) const
{
	RECT rcBorder;

	// Top  Part
	if (lpRectNew->top > lpRectOld->top)
	{
		::SetRect(&rcBorder, lpRectOld->left, lpRectOld->top, 
			lpRectOld->right, lpRectNew->top);
		::InvalidateRect(m_hParent, &rcBorder, FALSE);
	}

	// Bottom Part
	if (lpRectNew->bottom < lpRectOld->bottom)
	{
		::SetRect(&rcBorder, lpRectOld->left, lpRectNew->bottom, 
			lpRectOld->right, lpRectOld->bottom);
		::InvalidateRect(m_hParent, &rcBorder, FALSE);
	}

	// Left Part
	if (lpRectNew->left > lpRectOld->left)
	{
		::SetRect(&rcBorder, lpRectOld->left, lpRectOld->top,
			lpRectNew->left, lpRectOld->bottom);
		::InvalidateRect(m_hParent, &rcBorder, FALSE);
	}

	// Right Part
	if (lpRectNew->right < lpRectOld->right)
	{
		::SetRect(&rcBorder, lpRectNew->right, lpRectOld->top,
			lpRectOld->right, lpRectOld->bottom);
		::InvalidateRect(m_hParent, &rcBorder, FALSE);
	}
}


//////////////////////////////////////////////////////////////////////////
// support for size grip

void CSizingSP::CreateSizeGrip()
{
	if (m_hParent == NULL || m_hSizeGrip != NULL)
		return;

	RECT rcClient;
	::GetClientRect(m_hParent, &rcClient);
	int cxGrip = ::GetSystemMetrics(SM_CXVSCROLL);
	int cyGrip = ::GetSystemMetrics(SM_CYHSCROLL);

	m_hSizeGrip = ::CreateWindow(_T("ScrollBar"), _T(""),
		WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | SBS_SIZEGRIP, 
		rcClient.right - cxGrip, rcClient.bottom - cyGrip, cxGrip, cyGrip, 
		m_hParent, NULL, NULL, 0);
	double dRate[4] = {1.0, 1.0, 1.0, 1.0};
	Append(m_hSizeGrip, dRate);
}


BOOL CSizingSP::AnalyseCmdShowSizeGrip() const
{
	if( m_nCmdShowSizeGrip == SW_HIDE )
		return FALSE;
	if( m_nCmdShowSizeGrip == SW_SHOW )
		return TRUE;

	if( m_clrBackground != g_clrBtnFace )
		return FALSE;

	WINDOWPLACEMENT wp;
	ZeroMemory( &wp, sizeof( wp ) );

	::GetWindowPlacement( m_hParent, &wp );
	if( wp.showCmd == SW_SHOWMAXIMIZED )
		return FALSE;

	return TRUE;
}


//////////////////////////////////////////////////////////////////////////
// Input analysis


// Get reposition information from a string
// initialize the array before call this function

BOOL CSizingSP::GetRatesFromStr( LPCTSTR lpszStr, HWND hCtrl, double* pdRate ) const
{
	CString str = lpszStr;
	str.Remove( _T( ' ' ) );
	str.MakeUpper();

	int nLen = str.GetLength();
	int posSpr = -1;
	int posSprNext = 0;
	while( posSprNext != -1 )
	{
		CString strSec;
		posSprNext = str.Find( _T( '+' ), posSpr + 1 );
		if( posSprNext == -1 )
			strSec = str.Mid( posSpr + 1, nLen - posSpr - 1 );
		else
			strSec = str.Mid( posSpr + 1, posSprNext - posSpr - 1 );

		if( !GetRatesFromSec( strSec, hCtrl, pdRate ) )
			return FALSE;

		posSpr = posSprNext;
	}
	return TRUE;
}


// dCoef is the factor which you can specify to determine the degree
// how the controls' position is affected by the parent window border.
// dCoef is set to 1.0 when you don't specify a value for it.
// 
BOOL CSizingSP::GetRatesFromSec( LPCTSTR lpszSec, HWND hCtrl, double* pdRate ) const
{
	CString strWord;
	double dCoef;
	GetCoefficientAndWord( lpszSec, strWord, dCoef );

	if( strWord.IsEmpty() )
		strWord = _T( "R" );

	// synchronize with the right/bottom border of the window in 
	// horizontal/vertical direction.
	if( strWord == _T( "P" ) || strWord == _T( "PXY" ) )
		strWord = _T( "XY" );
	else if( strWord == _T( "PX" ) )
		strWord = _T( "X" );
	else if( strWord == _T( "PY" ) )
		strWord = _T( "Y" );

	if( strWord == _T( "X" ) || strWord == _T( "Y" ) || strWord == _T( "XY" ) )
	{
		if( strWord != _T( "Y" ) )
			pdRate[0] = pdRate[1] = dCoef;
		if( strWord != _T( "X" ) )
			pdRate[2] = pdRate[3] = dCoef;
	}

	// only the right/bottom border of the controls will synchronize with 
	// the right/bottom border of the window in horizontal/vertical direction.
	else if( strWord == _T( "S" ) || strWord == _T( "SXY" )
		|| strWord == _T( "SX" ) || strWord == _T( "SY" ) )
	{
		if( strWord != _T( "SY" ) )
		{
			pdRate[0] = 0.0;
			pdRate[1] = dCoef;
		}
		if( strWord != _T( "SX" ) )
		{
			pdRate[2] = 0.0;
			pdRate[3] = dCoef;
		}
	}

	// keep the position and size of the controls and the window's client area 
	// be in same proportion when resizing the window.
	else if( strWord == _T( "R" ) || strWord == _T( "RXY" )
		|| strWord == _T( "RX" ) || strWord == _T( "RY" ) )
	{
		ASSERT( ::IsChild( m_hParent, hCtrl ) );

		RECT rcInitCtrl;
		CListElement const* pElement = Find( hCtrl );
		if( pElement != NULL )
			rcInitCtrl = pElement->m_rcInit;
		else
			GetWindowRectClient( m_hParent, hCtrl, &rcInitCtrl );

		if( strWord != _T( "RY" ) )
		{
			pdRate[0] = dCoef*(rcInitCtrl.left - m_rcInitClt.left) / (m_rcInitClt.right - m_rcInitClt.left);
			pdRate[1] = dCoef*(rcInitCtrl.right - m_rcInitClt.left) / (m_rcInitClt.right - m_rcInitClt.left);
		}
		if( strWord != _T( "RX" ) )
		{
			pdRate[2] = dCoef*(rcInitCtrl.top - m_rcInitClt.top) / (m_rcInitClt.bottom - m_rcInitClt.top);
			pdRate[3] = dCoef*(rcInitCtrl.bottom - m_rcInitClt.top) / (m_rcInitClt.bottom - m_rcInitClt.top);
		}
	}

	// Error
	else
	{
		CString strErr;
		strErr.Format( _T( "'%s' is not a predefined keyword." ), (LPCTSTR)strWord );
		AfxMessageBox( strErr, MB_OK | MB_ICONEXCLAMATION );
		return FALSE;
	}

	return TRUE;
}


// Get coefficient and word from a string such as "0.5X" or "(-1.02)XY" 
// or "(-12.36R)"

void CSizingSP::GetCoefficientAndWord( LPCTSTR lpszSection, CString& strWord, double& dCoef ) const
{
	strWord = lpszSection;
	dCoef = 1.0;

	CString str( lpszSection );
	int nLen = str.GetLength();
	if( nLen < 1 )
		return;

	CString strCoef;
	TCHAR ch = str.GetAt( 0 );
	if( ch == _T( '(' ) )
	{
		int nIndex = str.ReverseFind( _T( ')' ) );
		if( nIndex == -1 )				// No the other bracket
			return;
		if( nIndex == nLen - 1 )			// All the string is bracketed
		{
			GetCoefficientAndWord( strWord.Mid( 1, nLen - 2 ), strWord, dCoef );
			return;
		}
		strWord = str.Right( nLen - nIndex - 1 );
		strCoef = str.Mid( 1, nIndex - 1 );
		if( !strCoef.IsEmpty() )
			dCoef = ::_ttof( strCoef );
		return;
	}

	int i = 0;
	for( i = 0; i < nLen; i++ )
	{
		ch = str.GetAt( i );
		if( ::isdigit( ch ) || ch == _T( '.' ) || ch == _T( 'E' )
			|| ch == _T( '+' ) || ch == _T( '-' ) || ::isspace( ch ) )
			strCoef += ch;
		else
		{
			strWord = str.Right( nLen - i );
			break;
		}
	}
	if( i == nLen )
		strWord.Empty();
	if( !strCoef.IsEmpty() )
		dCoef = ::_ttof( strCoef );
}


// Standardize ID: correct the tab order of two specified controls' ID
// and replace the NULL value to a proper real ID.

void CSizingSP::StandardizeID( UINT& nReIDS, UINT& nReIDE ) const
{
	if( nReIDS == NULL )
		nReIDS = ::GetDlgCtrlID( ::GetWindow( m_hParent, GW_CHILD ) );

	UINT nIDTmp = NULL;
	BOOL bOrderOK = FALSE;
	for( HWND hCtrlTmp = ::GetWindow( m_hParent, GW_CHILD ); hCtrlTmp != NULL;
		hCtrlTmp = ::GetNextWindow( hCtrlTmp, GW_HWNDNEXT ) )
	{
		nIDTmp = ::GetDlgCtrlID( hCtrlTmp );
		if( bOrderOK == FALSE )
		{
			if( nIDTmp == nReIDS )	// Order is OK
				bOrderOK = TRUE;	// Continue to loop to get the last control's ID
			if( nIDTmp == nReIDE )	// Correct the tab order
			{
				nIDTmp = nReIDS;
				nReIDS = nReIDE;
				nReIDE = nIDTmp;
				break;
			}
		}
	}
	if( nReIDE == NULL )
		nReIDE = nIDTmp;			// modify the last control's ID

	ASSERT( ::GetDlgItem( m_hParent, nReIDS ) != NULL );
	ASSERT( ::GetDlgItem( m_hParent, nReIDE ) != NULL );
}


int CSizingSP::GetCtrlCount( UINT nIDS, UINT nIDE ) const
{
	StandardizeID( nIDS, nIDE );

	int num = 0;
	for( HWND hCtrlTmp = ::GetDlgItem( m_hParent, nIDS ); hCtrlTmp != NULL;
		hCtrlTmp = ::GetNextWindow( hCtrlTmp, GW_HWNDNEXT ) )
	{
		num++;
		UINT nIDTmp = ::GetDlgCtrlID( hCtrlTmp );
		if( nIDTmp == nIDE )
			break;
	}
	return num;
}


//////////////////////////////////////////////////////////////////////////
// Calculate control rectangle 

void CSizingSP::GetCtrlFragRects(
	HWND hCtrl, LPCRECT rcCtrl, LPRECT lprc, int nSize) const
{
	if (nSize == CK_GROUPBOX)
		GetGrpBoxRgn(hCtrl, *rcCtrl, lprc);
	else if (nSize == CK_FRAMEPICTRUE)
		GetFrmPicRgn(hCtrl, *rcCtrl, lprc);
	else
		lprc[0] = lprc[1] = *rcCtrl;
}


int CSizingSP::GetControlKind(HWND hCtrl) const
{
	CString str;
	::GetClassName(hCtrl, str.GetBuffer(100), 100);
	str.ReleaseBuffer();

	UINT nStyle = ::GetWindowLong(hCtrl, GWL_STYLE);

	if (str == _T("Button") && (nStyle & 0x0000000FL) == BS_GROUPBOX)
		return CK_GROUPBOX;

	if ((str == _T("Static"))
		&& ( (nStyle & SS_BLACKFRAME) == SS_BLACKFRAME
		|| (nStyle & SS_GRAYFRAME) == SS_GRAYFRAME
		|| (nStyle & SS_WHITEFRAME) == SS_WHITEFRAME
		|| (nStyle & SS_ETCHEDFRAME) == SS_ETCHEDFRAME
		|| (nStyle & SS_ETCHEDFRAME) == SS_ETCHEDHORZ
		|| (nStyle & SS_ETCHEDFRAME) == SS_ETCHEDVERT ) )
		return CK_FRAMEPICTRUE;

	str += ',';
	CString strEnum = _T("	\
						 ListBox,			\
						 SysListView32,		\
						 SysTreeView32,		\
						 SysTabControl32,	\
						 ComboBoxEx32,		\
						 ");

	if (strEnum.Find(str) >= 0)
		return CK_SPECIALCTRLS;

	return CK_OTHERS;
}


void CSizingSP::GetWindowTextSize(HWND hWnd, SIZE& sz) const
{
	ASSERT(::IsWindow(hWnd));

	TCHAR str[MAX_PATH];
	::GetWindowText(hWnd, str, MAX_PATH);
	if (str[0] == _T('\0'))
		::_tcscpy_s(str, _T("a"));		// To avoid getting zero valued sz.

	HDC hDc = ::GetDC(hWnd);
	HFONT hFont = (HFONT)::SendMessage(hWnd, WM_GETFONT, 0, 0);
	HFONT hFontOld = (HFONT)::SelectObject(hDc, hFont);

	::GetTextExtentPoint32(hDc, str, (int)::_tcslen(str), &sz);
	sz.cx += 4;
	sz.cy += 2;

	::SelectObject(hDc, hFontOld);

	::ReleaseDC(hWnd, hDc);
}


//////////////////////////////////////////////////////////////////////////
// Group Box Support

//////////////////////////////////////////////////////////////////////////
//
//void CSizingSP::GetGrpBoxRgn(
//	HWND hCtrl, RECT const& rcCtrl, RECT* rcGBox) const
//
// The meanings of rcGBox's elements in a GrpBox, 0 means whole rectangle.
// The case of a frame picture control is similar.
//
//                    _________8_________
//                   |					 |
//                   6					 7
//                   |___4___[ 5 ]___4___|
//                   |	                 |
//                   |					 |
//                   |					 |
//                   2					 3
//                   |					 |
//                   |					 |
//                   |					 |
//                   |_________1_________|
//
//
//////////////////////////////////////////////////////////////////////////

void CSizingSP::GetGrpBoxRgn(HWND hCtrl, RECT const& rcCtrl, RECT* rcGBox) const
{
	ASSERT(::IsChild(m_hParent, hCtrl));
	ASSERT(GetControlKind(hCtrl) == CK_GROUPBOX);

	SIZE sz;
	sz.cx = sz.cy = 0;
	GetWindowTextSize(hCtrl, sz);

	// whole rect
	rcGBox[0] = rcCtrl;

	// bottom line
	::SetRect(&rcGBox[1],
		rcCtrl.left,		rcCtrl.bottom - 2,
		rcCtrl.right,		rcCtrl.bottom);

	// left line
	::SetRect(&rcGBox[2],
		rcCtrl.left,		rcCtrl.top + sz.cy / 2 - 1,
		rcCtrl.left + 2,	rcCtrl.bottom); 

	// right line
	::SetRect(&rcGBox[3],
		rcCtrl.right - 2,	rcCtrl.top + sz.cy / 2 - 1,
		rcCtrl.right,		rcCtrl.bottom); 

	// upper line
	::SetRect(&rcGBox[4],
		rcCtrl.left,		rcCtrl.top + sz.cy / 2 - 1,
		rcCtrl.right,		rcCtrl.top + sz.cy / 2 + 1);

	// text rect 
	RECT rcText;
	::SetRect(&rcText, 0, 0, sz.cx, sz.cy);

	int cxOffset = 7;

	LONG lStyle = ::GetWindowLong(hCtrl, GWL_STYLE);
	lStyle &= 0x00000300L;	// get the alignment property

	int nAlign = -1;		// for later user
	if (lStyle == BS_CENTER)
	{
		cxOffset = (rcCtrl.right - rcCtrl.left - sz.cx) / 2;
		nAlign = 0;
	}
	else if (lStyle == BS_RIGHT)
	{
		cxOffset = rcCtrl.right - rcCtrl.left - sz.cx - 7;
		nAlign = 1;
	}

	::OffsetRect(&rcText, 
		rcCtrl.left + cxOffset - rcText.left, rcCtrl.top - rcText.top);

	TCHAR str[MAX_PATH];
	::GetWindowText(hCtrl, str, MAX_PATH);
	if (str[0] != '\0')
		rcGBox[5] = rcText;
	else
		::SetRectEmpty(&rcGBox[5]);

	// Additional adjustment needed if the group box is not created 
	// with the default style

	int adj = 0;
	LONG lStyleEx = ::GetWindowLong(hCtrl, GWL_EXSTYLE);
	if ((lStyleEx & WS_EX_CLIENTEDGE) == WS_EX_CLIENTEDGE)
		adj += 2;
	if ((lStyleEx & WS_EX_DLGMODALFRAME) == WS_EX_DLGMODALFRAME) 
		adj += 3;
	if ((lStyleEx & WS_EX_STATICEDGE) == WS_EX_STATICEDGE &&
		(lStyleEx & WS_EX_DLGMODALFRAME) != WS_EX_DLGMODALFRAME) 
		adj += 1;

	if (adj != 0)
		AdjGrpBoxRgn(rcGBox, adj, nAlign);
	else
	{
		for (int i = 6; i < 9; i++)
			::SetRectEmpty(&rcGBox[i]);
	}
}

void CSizingSP::AdjGrpBoxRgn(RECT* pRc, int a, int nA) const
{
	ASSERT(nA >= -1 && nA <= 1);

	InflateRect( &pRc[1], 0,     a,     0,   0);
	InflateRect( &pRc[2], 0,    -a,     a,   0);
	InflateRect( &pRc[3], a,    -a,     0,   0);

	if (!::IsRectEmpty(&pRc[4]))
		InflateRect(&pRc[4], 0, -a, 0, a);

	InflateRect(&pRc[5], a*nA, -a, -a*nA,   a);

	::SetRect(&pRc[6], pRc[0].left,	     pRc[0].top, pRc[0].left + a, pRc[4].top);
	::SetRect(&pRc[7], pRc[0].right - a, pRc[0].top, pRc[0].right,	  pRc[4].top);
	::SetRect(&pRc[8], pRc[0].left,		 pRc[0].top, pRc[0].right,	  pRc[0].top + a);
}

//////////////////////////////////////////////////////////////////////////
// Frame Picture Control Support

void CSizingSP::GetFrmPicRgn(
							 HWND hCtrl, RECT const& rcCtrl, RECT* rcPic) const
{
	ASSERT(::IsChild(m_hParent, hCtrl));
	ASSERT(GetControlKind(hCtrl) == CK_FRAMEPICTRUE);

	// whole rect
	rcPic[0] = rcCtrl;

	int szBoder = 1;
	UINT nStyle = ::GetWindowLong(hCtrl, GWL_STYLE);
	if ((nStyle & SS_ETCHEDFRAME) == SS_ETCHEDFRAME)
		szBoder = 2;

	// bottom line
	::SetRect(&rcPic[1],
		rcCtrl.left,	rcCtrl.bottom - szBoder,
		rcCtrl.right,	rcCtrl.bottom);

	// left line
	::SetRect(&rcPic[2],
		rcCtrl.left,			rcCtrl.top,
		rcCtrl.left + szBoder,	rcCtrl.bottom);

	// right line
	::SetRect(&rcPic[3],
		rcCtrl.right - szBoder, rcCtrl.top,
		rcCtrl.right,			rcCtrl.bottom); 

	// top line
	::SetRect(&rcPic[4],
		rcCtrl.left,	rcCtrl.top,
		rcCtrl.right,	rcCtrl.top + szBoder);

	// Additional adjustment needed if the FrmPic is not created 
	// with the default style

	int adj = 0;
	LONG lStyleEx = ::GetWindowLong(hCtrl, GWL_EXSTYLE);
	if ((lStyleEx & WS_EX_CLIENTEDGE) == WS_EX_CLIENTEDGE)
		adj += 2;
	if ((lStyleEx & WS_EX_DLGMODALFRAME) == WS_EX_DLGMODALFRAME) 
		adj += 3;
	if ((lStyleEx & WS_EX_STATICEDGE) == WS_EX_STATICEDGE &&
		(lStyleEx & WS_EX_DLGMODALFRAME) != WS_EX_DLGMODALFRAME) 
		adj += 1;

	if (adj != 0)
		AdjFrmPicRgn(rcPic, adj);
}

void CSizingSP::AdjFrmPicRgn(RECT* rcPic, int a) const
{
	InflateRect(&rcPic[1], 0,  a,  0,  0);
	InflateRect(&rcPic[2], 0,  0,  a,  0);
	InflateRect(&rcPic[3], a,  0,  0,  0);
	InflateRect(&rcPic[4], 0,  0,  0,  a);
}

//////////////////////////////////////////////////////////////////////////
// Line Table Operations

void CSizingSP::Append(HWND hCtrl, double const* pdRate)
{
	ASSERT(::IsChild(m_hParent, hCtrl));

	RECT rcInit;
	CListElement const* pElement = Find(hCtrl);
	if (pElement != NULL)
	{
		if (hCtrl == m_hSizeGrip)
			return;

		rcInit = pElement->m_rcInit;
		Remove(hCtrl);
	}
	else
	{
		GetWindowRectClient(m_hParent, hCtrl, &rcInit);
	}

	CListElement* pTmp = new CListElement(hCtrl, rcInit, pdRate, NULL);
	if (m_pHead == NULL)
		m_pHead = pTmp;
	else
		m_pTail->m_pNext = pTmp;
	m_pTail = pTmp;
}

BOOL CSizingSP::Remove(HWND hCtrl)
{
	CListElement* pTmp    = m_pHead;
	CListElement* pPreTmp = NULL;
	while(pTmp != NULL && pTmp->m_hCtrl != hCtrl)
	{
		pPreTmp = pTmp;
		pTmp = pTmp->m_pNext;
	}

	if (pTmp == NULL)
		return FALSE;

	if (pTmp == m_pHead)
		m_pHead = pTmp->m_pNext;
	else 
		pPreTmp->m_pNext = pTmp->m_pNext;

	if (pTmp == m_pTail)
		m_pTail = pPreTmp;

	MYDELETE(pTmp);
	return TRUE;
}

CListElement const* CSizingSP::Find(HWND hCtrl) const
{
	CListElement* pTmp = m_pHead;
	while(pTmp != NULL && pTmp->m_hCtrl != hCtrl)
		pTmp = pTmp->m_pNext;
	return pTmp;
}

//////////////////////////////////////////////////////////////////////////
// extended APIs

BOOL CSizingSP::GetWindowRectClient(HWND hParent, HWND hWnd, LPRECT lpRect) const
{
	if(::GetWindowRect(hWnd, lpRect) == FALSE)
		return FALSE;

	POINT ptLT;
	POINT ptRB;

	ptLT.x = lpRect->left;
	ptLT.y = lpRect->top;
	ptRB.x = lpRect->right;
	ptRB.y = lpRect->bottom;

	if (!::ScreenToClient(hParent, &ptLT) || !::ScreenToClient(hParent, &ptRB))
		return FALSE;

	lpRect->left = ptLT.x;
	lpRect->top = ptLT.y;
	lpRect->right = ptRB.x;
	lpRect->bottom = ptRB.y;

	return TRUE;
}

void CSizingSP::InflateRect(LPRECT lprc, int dl, int dt, int dr, int db) const
{
	lprc->left   -= dl;
	lprc->top    -= dt;
	lprc->right  += dr;
	lprc->bottom += db;
}

BOOL CSizingSP::IsRectEqual(LPCRECT lprc1, LPCRECT lprc2) const
{
	return lprc1->left   == lprc2->left 
		&& lprc1->right	 == lprc2->right
		&& lprc1->top	 == lprc2->top 
		&& lprc1->bottom == lprc2->bottom;
}

