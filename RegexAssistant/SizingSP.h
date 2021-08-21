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
// SizingSP.h 

#pragma once


//////////////////////////////////////////////////////////////////////////
// class CListElement

class CSizingSP;

class CListElement
{
	friend CSizingSP;

	// Constructor
private:
	CListElement(HWND hCtrl, RECT const& rcInit, double const* pdRate, 
		CListElement* pNext);

	// Data
private:
	HWND  			m_hCtrl;
	RECT  			m_rcInit;
	double			m_dRate[4];
	CListElement*	m_pNext;
};




enum CSIZINGSP_DATATYPE {DATATYPE_STRING = 0, DATATYPE_NUMBER = 1};
//////////////////////////////////////////////////////////////////////////
// class CSizingSP

class CSizingSP
{
	// Constructor and destructor
public:
	CSizingSP();
	void Initialize(HWND hParent);
	virtual ~CSizingSP();

	// Operations
public:
	// Add resizable controls
	void AddControl(UINT nID, LPCVOID lpData, int nDataType);
	void AddControl(HWND hCtrl, LPCVOID lpData = NULL, int nDataType = DATATYPE_STRING);
	void AddControlRange(UINT nIDS, UINT nIDE, LPCVOID lpData, int nDataType);
	void AddControlArray(UINT nIDS, UINT nIDE, double dCtrlGain = 0.0, 
		double dBorderIntervalRate = 1.0, BOOL bHori = TRUE, 
		LPCTSTR lpszStrExtra = _T("Y"));

	// Resize support
	BOOL ResizeControls() const;
	BOOL EraseBkgnd(HDC hDC);
	void UpdateGBoxFPic() const;

	// Size grip for dialog
	BOOL ShowSizeGrip(int nCmdShow = SW_SHOWDEFAULT);

	// window background color support
	COLORREF GetBgColor() const;
	void SetBgColor(COLORREF clrColor);

	// Implementation
private:
	// redraw controls support
	void InvalidateCtrl(HWND hCtrl, RECT const& rcCtrlOld, RECT const& rcCtrl) const;
	void InvalidateCtrlBorder(LPCRECT lpRectOld, LPCRECT lpRectNew) const;

	// support for size grip
	void CreateSizeGrip();
	BOOL AnalyseCmdShowSizeGrip() const;

	// Input analysis
	BOOL GetRatesFromStr(LPCTSTR lpszStr, HWND hCtrl, double* pdRate) const;
	BOOL GetRatesFromSec(LPCTSTR lpszSec, HWND hCtrl, double* pdRate) const;
	void GetCoefficientAndWord(
		LPCTSTR lpszSection, CString& strWord, double& dCoef) const;
	void StandardizeID(UINT& nReIDS, UINT& nReIDE) const;
	int GetCtrlCount(UINT nIDS, UINT nIDE) const;

	// Calculate control rectangle 
	void GetCtrlFragRects(HWND hCtrl, LPCRECT rcCtrl, LPRECT lprc, int nSize) const;
	int GetControlKind(HWND hCtrl) const;
	void GetWindowTextSize(HWND hWnd, SIZE& sz) const;

	// Group Box Support
	void GetGrpBoxRgn(HWND hCtrl, RECT const& rcCtrl, RECT* rcGBox) const;
	void AdjGrpBoxRgn(RECT* rcGBox, int adj, int nAlign) const;

	// Frame Picture Control Support
	void GetFrmPicRgn(HWND hCtrl, RECT const& rcCtrl, RECT* rcPic) const;
	void AdjFrmPicRgn(RECT* rcPic, int adj) const;

	// Line Table Operations
	void Append(HWND hCtrl, double const* pdRate);
	BOOL Remove(HWND hCtrl);
	CListElement const* Find(HWND hCtrl) const;

	// Extended APIs
	BOOL GetWindowRectClient(HWND hParent, HWND hWnd, LPRECT lpRect) const;
	void InflateRect(LPRECT lprc, int dl, int dt, int dr, int db) const;
	BOOL IsRectEqual(LPCRECT lprc1, LPCRECT lprc2) const;

// Data
private:
	HWND			m_hParent;				// handle of the parent window
	RECT			m_rcInitClt;			// initial client rect of parent

	CListElement*	m_pHead;				// head of the control list
	CListElement*	m_pTail;				// tail of the control list

	HWND			m_hSizeGrip;			// handle of the size grip
	int				m_nCmdShowSizeGrip;		// show command of size grip
	
	COLORREF		m_clrBackground;		// background color of parent	
	
	HRGN 			m_hRgnErase;			// region-object for erase
	HRGN 			m_hRgnCtrl;				// region-object for controls
};


//////////////////////////////////////////////////////////////////////////
// Macro definitions

#define MYDELETE(x)				delete x;	x = NULL;
#define MYDELETEARRAY(x)		delete []x;	x = NULL;

static DWORD g_clrBtnFace =		::GetSysColor(COLOR_BTNFACE);



#define DECLARE_SIZING_SUPPORT()					\
protected:																			\
	CSizingSP		m_objSizingSP;													\
public:																				\
	void Initialize(HWND hWnd);														\
	void AddControl(UINT nID,	double dPGain, double dSGain);						\
	void AddControl(																\
	UINT nID, double dPXGain, double dSXGain, double dPYGain, double dSYGain);		\
	void AddControl(UINT nID, LPCTSTR lpszStr = NULL);								\
	void AddControl(HWND hCtrl, LPCTSTR lpszStr = NULL);							\
																					\
	void AddControlRange(UINT nIDS, UINT nIDE, double dPGain, double dSGain);		\
	void AddControlRange(															\
	UINT nIDS, UINT nIDE,															\
	double dPXGain, double dSXGain, double dPYGain, double dSYGain);				\
	void AddControlRange(UINT nIDS, UINT nIDE, LPCTSTR lpszStr = NULL);				\
																					\
	void AddControlArray(															\
	UINT nIDS, UINT nIDE, 															\
	double dCtrlGain = 0.0, double dBorderIntervalRate = 1.0,						\
	BOOL bHori = TRUE, LPCTSTR lpszStrExtra = _T("Y"));								\


#define IMPLEMENT_SIZING_SUPPORT(class_name)		\
void class_name::Initialize(HWND hWnd)												\
{																					\
	m_objSizingSP.Initialize(hWnd);													\
}																					\
																					\
/* Add a single control or all the controls (when nID == NULL)						\
*/																					\
void class_name::AddControl(UINT nID, double dPGain, double dSGain)					\
{																					\
	AddControl(nID, dPGain, dSGain, dPGain, dSGain);								\
}																					\
																					\
void class_name::AddControl(														\
	UINT nID,double dPXGain, double dSXGain, double dPYGain, double dSYGain)		\
{																					\
	double dRate[4] = {dPXGain, dPXGain + dSXGain, dPYGain, dPYGain + dSYGain};		\
	m_objSizingSP.AddControl(nID, dRate, DATATYPE_NUMBER);							\
}																					\
																					\
void class_name::AddControl(UINT nID, LPCTSTR lpszStr /*= NULL*/)					\
{																					\
	m_objSizingSP.AddControl(nID, lpszStr, DATATYPE_STRING);						\
}																					\
																					\
void class_name::AddControl(HWND hCtrl, LPCTSTR lpszStr /*= NULL*/)					\
{																					\
	m_objSizingSP.AddControl(hCtrl, lpszStr, DATATYPE_STRING);						\
}																					\
																					\
																					\
/* Add a control-range with the same settings which IDs range from nIDS to nIDE.	\
*/																					\
void class_name::AddControlRange(													\
	UINT nIDS, UINT nIDE, double dPGain, double dSGain)								\
{																					\
	AddControlRange(nIDS, nIDE, dPGain, dSGain, dPGain, dSGain);					\
}																					\
																					\
void class_name::AddControlRange(													\
	UINT nIDS, UINT nIDE,															\
	double dPXGain, double dSXGain, double dPYGain, double dSYGain)					\
{																					\
	double dRate[4] = {dPXGain, dPXGain + dSXGain, dPYGain, dPYGain + dSYGain};		\
	m_objSizingSP.AddControlRange(nIDS, nIDE, dRate, DATATYPE_NUMBER);				\
}																					\
																					\
void class_name::AddControlRange(UINT nIDS, UINT nIDE, LPCTSTR lpszStr /*= NULL*/)	\
{																					\
	m_objSizingSP.AddControlRange(nIDS, nIDE, lpszStr, DATATYPE_STRING);			\
}																					\
																					\
/* Add a control-range which can be rearranged with different settings				\
*/																					\
void class_name::AddControlArray(													\
	UINT nIDS, UINT nIDE, double dC /*= 0.0*/, double dR /*= 1.0*/,					\
	BOOL bHori /*= TRUE*/,	LPCTSTR lpszStrExtra /*= _T("Y")*/)						\
{																					\
	m_objSizingSP.AddControlArray(nIDS, nIDE, dC, dR, bHori, lpszStrExtra);			\
}																					\

