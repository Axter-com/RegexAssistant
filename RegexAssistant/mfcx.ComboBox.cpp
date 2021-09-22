/*
	Copyright (C) 2021  David Maisonave
	The mfcx source code is free software. You can redistribute it and/or modify it under the terms of the GNU General Public License.

	Purpose:
		See header file mfcx.ComboBox.h

	Usage Details:
		See header file mfcx.ComboBox.h
*/
#include "pch.h"
#include "mfcx.ComboBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

COLORREF GetColor( COLORREF colorref, int DefaultID )
{
	return (colorref == mfcx::DEFAULT_COLORS) ? GetSysColor( DefaultID ) : colorref;
}

namespace mfcx
{
	ComboBox::ComboBox( const ColorRefSet &colorrefset )
		:m_ToolTipItemRecievedStr( FALSE ), m_bLastCB_Sel( -2 ), m_bLastValid_Sel( 0 )
		, m_DisabledColor( GetColor( colorrefset.DisabledColor, COLOR_WINDOWTEXT ) ), m_DisabledBkColor( GetColor( colorrefset.DisabledBkColor, COLOR_WINDOW ) )
		, m_EnabledColor( GetColor( colorrefset.EnabledColor, COLOR_WINDOWTEXT ) ), m_EnabledBkColor( GetColor( colorrefset.EnabledBkColor, COLOR_WINDOW ) )
		, m_EnabledSelectColor( GetColor( colorrefset.EnabledSelectColor, COLOR_HIGHLIGHTTEXT ) ), m_EnabledSelectBkColor( GetColor( colorrefset.EnabledSelectBkColor, COLOR_HIGHLIGHT ) )
		, m_ListBox( new ComboBoxListBox() ), m_DoToolTipPopupWhenItemStringTooWide( FALSE )
	{
		Initiate();
	}

	ComboBox::ComboBox( const CString &ToolTipString, const ColorRefSet &colorrefset )
		:m_ToolTipItemRecievedStr( FALSE ), m_bLastCB_Sel( -2 ), m_bLastValid_Sel( 0 ), m_ToolTipString( ToolTipString )
		, m_DisabledColor( GetColor( colorrefset.DisabledColor, COLOR_WINDOWTEXT ) ), m_DisabledBkColor( GetColor( colorrefset.DisabledBkColor, COLOR_WINDOW ) )
		, m_EnabledColor( GetColor( colorrefset.EnabledColor, COLOR_WINDOWTEXT ) ), m_EnabledBkColor( GetColor( colorrefset.EnabledBkColor, COLOR_WINDOW ) )
		, m_EnabledSelectColor( GetColor( colorrefset.EnabledSelectColor, COLOR_HIGHLIGHTTEXT ) ), m_EnabledSelectBkColor( GetColor( colorrefset.EnabledSelectBkColor, COLOR_HIGHLIGHT ) )
		, m_ListBox( new ComboBoxListBox() ), m_DoToolTipPopupWhenItemStringTooWide( FALSE )
	{
		Initiate();
	}

	void ComboBox::Initiate()
	{
		m_ListBox->SetParent( this );
	}

	ComboBox::~ComboBox()
	{
	}

	// ComboBox static members
	const UINT ComboBox::nMessage = ::RegisterWindowMessage( _T( "ComboSelEndOK" ) );
	const ColorRefSet ComboBox::DefaultColors;
	const TextColorRefSet ComboBox::DefaultTextColors( DEFAULT_COLORS );
	const TextBackGroundColorRefSet ComboBox::DefaultTextBkColors( DEFAULT_COLORS );

	BEGIN_MESSAGE_MAP( ComboBox, CComboBox )
		//{{AFX_MSG_MAP(ComboBox)
		ON_WM_CHARTOITEM()
		ON_CONTROL_REFLECT( CBN_SELENDOK, OnSelendok )
		ON_MESSAGE( WM_CTLCOLORLISTBOX, OnCtlColor )
		ON_REGISTERED_MESSAGE( nMessage, OnRealSelEndOK )
		//}}AFX_MSG_MAP
	END_MESSAGE_MAP()

	void ComboBox::PreSubclassWindow()
	{
		_ASSERT_EXPR( ((GetStyle()&(CBS_OWNERDRAWFIXED | CBS_HASSTRINGS)) == (CBS_OWNERDRAWFIXED | CBS_HASSTRINGS)),
					  _T( "This ComboBox control requires CBS_OWNERDRAWFIXED and CBS_HASSTRINGS.  Look at this ComboBox's properties settings under [Behavior], and set [Has String] to 'true' and set [Owner Draw] to 'Fixed'" ) );
		UpdateMainTooltip( m_ToolTipString );

		m_hwndItemTip.CreateEx( WS_EX_TOPMOST, TOOLTIPS_CLASS, NULL,
								WS_POPUP | TTS_ALWAYSTIP | TTS_BALLOON,
								CW_USEDEFAULT, CW_USEDEFAULT,
								CW_USEDEFAULT, CW_USEDEFAULT,
								m_ListBox->GetSafeHwnd(), NULL );

		CComboBox::PreSubclassWindow();
	}

	COLORREF ComboBox::GetDisabledItemTextColor( BOOL IsDisabled, BOOL IsItemSelected, const TextColorRefSet &Superseded )
	{
		const COLORREF SelectedColor = (Superseded.EnabledSelectColor == DEFAULT_COLORS) ? m_EnabledSelectColor : Superseded.EnabledSelectColor;
		if ( !IsDisabled && IsItemSelected )
			return SelectedColor;
		const COLORREF DisabledColor = (Superseded.DisabledColor == DEFAULT_COLORS) ? m_DisabledColor : Superseded.DisabledColor;
		const COLORREF EnabledColor = (Superseded.EnabledColor == DEFAULT_COLORS) ? m_EnabledColor : Superseded.EnabledColor;
		const COLORREF TextColor = IsDisabled ? DisabledColor : EnabledColor;
		return TextColor;
	}

	COLORREF ComboBox::GetDisabledItemTextBkColor( BOOL IsDisabled, BOOL IsItemSelected, const TextBackGroundColorRefSet &Superseded )
	{
		const COLORREF SelectedBkColor = (Superseded.EnabledSelectBkColor == DEFAULT_COLORS) ? m_EnabledSelectBkColor : Superseded.EnabledSelectBkColor;
		if ( !IsDisabled && IsItemSelected )
			return SelectedBkColor;
		const COLORREF DisabledBkColor = (Superseded.DisabledBkColor == DEFAULT_COLORS) ? m_DisabledBkColor : Superseded.DisabledBkColor;
		const COLORREF EnabledBkColor = (Superseded.EnabledBkColor == DEFAULT_COLORS) ? m_EnabledBkColor : Superseded.EnabledBkColor;
		const COLORREF TextBkColor = IsDisabled ? DisabledBkColor : EnabledBkColor;
		return TextBkColor;
	}

	void ComboBox::DrawItem( LPDRAWITEMSTRUCT lpDrawItemStruct )
	{
		CDC* pDC = CDC::FromHandle( lpDrawItemStruct->hDC );
		RECT &rc = lpDrawItemStruct->rcItem;
		BOOL IsItemSelected = (lpDrawItemStruct->itemState & ODS_SELECTED) ? TRUE : FALSE;
		COLORREF oldTextColor = DEFAULT_COLORS;
		COLORREF oldBkColor = DEFAULT_COLORS;
		CFont* pOldFont = NULL;
		CString strText;

		if ( ((LONG)(lpDrawItemStruct->itemID) >= 0) &&
			(lpDrawItemStruct->itemAction & (ODA_DRAWENTIRE | ODA_SELECT)) )
		{
			ComboBoxItemDetails comboboxitemdetails;
			BOOL IsItemDisabled = !IsWindowEnabled() || !IsItemEnabled(lpDrawItemStruct->itemID, &comboboxitemdetails );
			CString ToolTipItem = comboboxitemdetails.ToolTipStr;
			COLORREF newTextColor = GetDisabledItemTextColor( IsItemDisabled, IsItemSelected, TextColorRefSet(comboboxitemdetails.DisabledColor, comboboxitemdetails.EnabledColor, comboboxitemdetails.EnabledSelectColor) );
			COLORREF newBkColor = GetDisabledItemTextBkColor( IsItemDisabled, IsItemSelected, TextBackGroundColorRefSet(comboboxitemdetails.DisabledBkColor, comboboxitemdetails.EnabledBkColor, comboboxitemdetails.EnabledSelectBkColor) );

			oldTextColor = pDC->SetTextColor( newTextColor );
			oldBkColor = pDC->SetBkColor( newBkColor );
			pOldFont = pDC->SelectObject( pDC->GetCurrentFont() );
			GetLBText( lpDrawItemStruct->itemID, strText );
			CRect rectClient( rc );
			pDC->ExtTextOut( rc.left,
							 rc.top,
							 ETO_OPAQUE, &rc,
							 strText, NULL );
			CSize TextSize = pDC->GetTextExtent( strText );

			if ( m_DoToolTipPopupWhenItemStringTooWide && (TextSize.cx > (lpDrawItemStruct->rcItem.right - lpDrawItemStruct->rcItem.left)) )
			{
				if ( ToolTipItem.IsEmpty() )
					ToolTipItem = strText;
				else
					ToolTipItem = _T( "[" ) + strText + _T( "] -- " ) + ToolTipItem;
			}
			m_ToolTipItemRecievedStr = (ToolTipItem.IsEmpty()) ? m_ToolTipItemRecievedStr : TRUE;

			if ( m_hwndItemTip.m_hWnd != NULL && m_ToolTipItemRecievedStr )
			{
				int nComboButtonWidth = ::GetSystemMetrics( SM_CXHTHUMB ) + 2;
				rectClient.right = rectClient.right - nComboButtonWidth;
				ClientToScreen( &rectClient );
				TOOLINFO toolInfo = {sizeof( toolInfo ), TTF_IDISHWND | TTF_SUBCLASS, m_ListBox->GetSafeHwnd(), reinterpret_cast<UINT_PTR>(m_ListBox->GetSafeHwnd())};
				if ( static_cast<int>(lpDrawItemStruct->itemID) != m_bLastCB_Sel || ToolTipItem.IsEmpty() )
				{
					m_bLastCB_Sel = static_cast<int>(lpDrawItemStruct->itemID);
					TOOLINFO toolInfo_toDelete = {sizeof( toolInfo_toDelete )};
					// Not sure why, but in order to remove the old tooltip from another item,
					// both the fetched tooltip and the about to be added tooltip need to get a deletion request.
					if ( m_hwndItemTip.SendMessage( TTM_GETCURRENTTOOL, 0, reinterpret_cast<LPARAM>(&toolInfo_toDelete) ) )
						m_hwndItemTip.SendMessage( TTM_DELTOOL, 0, reinterpret_cast<LPARAM>(&toolInfo_toDelete) );
					m_hwndItemTip.SendMessage( TTM_DELTOOL, 0, reinterpret_cast<LPARAM>(&toolInfo) );
				}

				if ( !ToolTipItem.IsEmpty() )
				{
					toolInfo.lpszText = const_cast<WCHAR*>(ToolTipItem.GetBuffer());
					rectClient.left += 1;
					rectClient.top += 3;
					m_hwndItemTip.SendMessage( TTM_TRACKPOSITION, 0, static_cast<LPARAM>(MAKELONG( rectClient.left, rectClient.top )) );
					m_hwndItemTip.SendMessage( TTM_UPDATETIPTEXT, 0, reinterpret_cast<LPARAM>(&toolInfo) );
					m_hwndItemTip.SendMessage( TTM_ADDTOOL, 0, reinterpret_cast<LPARAM>(&toolInfo) );
				}
			}
		} else if ( (LONG)(lpDrawItemStruct->itemID) < 0 )	// drawing edit text
		{
			oldTextColor = pDC->SetTextColor( GetDisabledItemTextColor( FALSE, IsItemSelected ) );
			oldBkColor = pDC->SetBkColor( GetDisabledItemTextBkColor( FALSE, IsItemSelected ) );
			GetWindowText( strText );
			pDC->ExtTextOut( rc.left + 2,
							 rc.top + 2,
							 ETO_OPAQUE, &rc,
							 strText, strText.GetLength(), NULL );
		}

		if ( oldTextColor != DEFAULT_COLORS && oldBkColor != DEFAULT_COLORS )
		{
			pDC->SetTextColor( oldTextColor );
			pDC->SetBkColor( oldBkColor );
			if ( pOldFont != NULL )
				pDC->SelectObject( pOldFont );
		}

		if ( (lpDrawItemStruct->itemAction & ODA_FOCUS) != 0 )
			pDC->DrawFocusRect( &lpDrawItemStruct->rcItem );
	}

	void ComboBox::MeasureItem( LPMEASUREITEMSTRUCT lpMeasureItemStruct )
	{
		UNREFERENCED_PARAMETER( lpMeasureItemStruct );
	}

	int ComboBox::OnCharToItem( UINT nChar, CListBox* pListBox, UINT nIndex )
	{
		int ret = CComboBox::OnCharToItem( nChar, pListBox, nIndex );
		if ( ret >= 0 && !IsItemEnabled( ret ) )
			return -2;
		else
			return ret;
	}

	void ComboBox::OnSelendok()
	{
		VerifyValidSelect();
		GetWindowText( m_strSavedText );
		PostMessage( nMessage );
	}

	LRESULT ComboBox::OnRealSelEndOK( WPARAM, LPARAM )
	{
		VerifyValidSelect();
		CString currentText;
		GetWindowText( currentText );
		int index = FindStringExact( -1, currentText );
		if ( index >= 0 && !IsItemEnabled( index ) )
		{
			SetWindowText( m_strSavedText );
			GetParent()->SendMessage( WM_COMMAND, MAKELONG( GetWindowLong( m_hWnd, GWL_ID ), CBN_SELCHANGE ), (LPARAM)m_hWnd );
		}
		return 0;
	}

	LRESULT ComboBox::OnCtlColor( WPARAM, LPARAM lParam )
	{
		if ( m_ListBox->m_hWnd == NULL && lParam != 0 && lParam != (LPARAM)m_hWnd )
			m_ListBox->SubclassWindow( (HWND)lParam );
		return Default();
	}

	void ComboBox::PostNcDestroy()
	{
		m_ListBox->Detach();
		CComboBox::PostNcDestroy();
	}

	void ComboBox::SetItemDetails( UINT nIndex, BOOL IsEnabled, CString ToolTipStr, const ColorRefSet &colorrefset )
	{
		if ( nIndex >= static_cast<DWORD>(GetCount()) )
			return;

		ComboBoxItemDetails* data = new ComboBoxItemDetails( IsEnabled, ToolTipStr, colorrefset.DisabledColor, colorrefset.DisabledBkColor, colorrefset.EnabledColor, colorrefset.EnabledBkColor, colorrefset.EnabledSelectColor, colorrefset.EnabledSelectBkColor );
		m_vComboBoxItemDetails.push_back( std::shared_ptr<ComboBoxItemDetails>( data ) );
		SetItemDataPtr( nIndex, static_cast<void*>(data) );
	}

	BOOL ComboBox::GetItemDetails( UINT nIndex, ComboBoxItemDetails &comboboxitemdetails ) 
	{
		BOOL FetchStatus = FALSE;
		IsItemEnabled( nIndex, &comboboxitemdetails,&FetchStatus );
		return FetchStatus;
	}

	BOOL ComboBox::IsItemEnabled( UINT nIndex, ComboBoxItemDetails *comboboxitemdetails, BOOL *FetchStatus ) // Returns item's enable status
	{
		BOOL DummyFetchStatus = FALSE;
		if ( FetchStatus == NULL )
			FetchStatus = &DummyFetchStatus;
		if ( nIndex >= static_cast<DWORD>(GetCount()) )
			return (*FetchStatus = FALSE);

		ComboBoxItemDetails* data = static_cast<ComboBoxItemDetails*>(GetItemDataPtr( nIndex ));
		if ( data != NULL )
		{
			*FetchStatus = TRUE;
			if ( comboboxitemdetails != NULL )
				*comboboxitemdetails = *data;
			return data->IsEnabled;
		}

		return (*FetchStatus = FALSE);
	}

	int ComboBox::AddString( const CString &str, BOOL IsEnabled, const CString &ToolTipStr, const ColorRefSet &colorrefset )
	{
		int nIndex = CComboBox::AddString( str );
		if ( nIndex != CB_ERR )
			SetItemDetails( nIndex, IsEnabled, ToolTipStr, colorrefset );

		return nIndex;
	}

	BOOL ComboBox::UpdateMainTooltip( const CString &value )
	{
		m_ToolTipString = value;
		if ( m_hwndTip.m_hWnd == NULL && !m_ToolTipString.IsEmpty() )
		{
			RECT rect;
			GetWindowRect( &rect );
			m_hwndTip.CreateEx( WS_EX_TOPMOST, TOOLTIPS_CLASS, NULL,
								WS_POPUP | TTS_ALWAYSTIP | TTS_BALLOON,
								rect.left, rect.top,
								rect.right - rect.left, rect.bottom - rect.top,
								GetParent()->m_hWnd, NULL );
		}

		if ( m_hwndTip.m_hWnd != NULL )
		{
			TOOLINFO toolInfo_toDelete = {sizeof( toolInfo_toDelete )};
			if ( m_hwndTip.SendMessage( TTM_GETCURRENTTOOL, 0, reinterpret_cast<LPARAM>(&toolInfo_toDelete) ) )
				m_hwndTip.SendMessage( TTM_DELTOOL, 0, reinterpret_cast<LPARAM>(&toolInfo_toDelete) );
			TOOLINFO toolInfo = {sizeof( toolInfo ), TTF_IDISHWND | TTF_SUBCLASS, GetParent()->m_hWnd, (UINT_PTR)m_hWnd};
			m_hwndTip.SendMessage( TTM_DELTOOL, 0, reinterpret_cast<LPARAM>(&toolInfo) );
			toolInfo.lpszText = m_ToolTipString.GetBuffer();
			return static_cast<BOOL>(m_hwndTip.SendMessage( TTM_ADDTOOL, 0, (LPARAM)&toolInfo ));
		}

		return FALSE;
	}

	void ComboBox::EnableWideStrPopup( BOOL Enabled )
	{
		m_DoToolTipPopupWhenItemStringTooWide = Enabled;
	}

	void ComboBox::VerifyValidSelect(int MoveSelect)
	{
		bool UpdateCompatibilitySettings = MoveSelect == 0 ? false : true;
		const int MaxItems = GetCount();
		if ( GO_TO_FIRSTVALIDITEM == MoveSelect )
		{
			SetCurSel( 0 );
			MoveSelect = 0;
		}
		else if ( GO_TO_LASTVALIDITEM == MoveSelect )
		{
			SetCurSel( MaxItems - 1 );
			MoveSelect = 0;
		}

		int nCurSel = GetCurSel() + MoveSelect;
		const BOOL bSelBack = MoveSelect ? (MoveSelect < 0) : (m_bLastValid_Sel > nCurSel);
		if ( nCurSel != CB_ERR && nCurSel < MaxItems )
		{
			if ( !IsItemEnabled( nCurSel ) )
			{
				for ( nCurSel; nCurSel < MaxItems;  (bSelBack ? --nCurSel : ++nCurSel) )
				{
					if ( IsItemEnabled( nCurSel ) )
						break;
				}
				if ( !IsItemEnabled( nCurSel ) )
				{
					nCurSel = GetCurSel();
					for ( nCurSel; nCurSel > -1; (bSelBack ? ++nCurSel : --nCurSel) )
					{
						if ( IsItemEnabled( nCurSel ) )
							break;
					}

				}
			}
			SetCurSel( nCurSel );
			m_bLastValid_Sel = GetCurSel();
		}
	}
	
	/////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////
	// ComboBoxListBox functions
	BEGIN_MESSAGE_MAP( ComboBoxListBox, CWnd )
		//{{AFX_MSG_MAP(ComboBoxListBox)
		ON_WM_LBUTTONUP()
		//}}AFX_MSG_MAP
	END_MESSAGE_MAP()

	void ComboBoxListBox::OnLButtonUp( UINT nFlags, CPoint point )
	{
		CRect rect;	GetClientRect( rect );
		if ( rect.PtInRect( point ) )
		{
			BOOL outside = FALSE;
			int index = ((CListBox *)this)->ItemFromPoint( point, outside );
			if ( !outside && !((ComboBox *)m_Parent)->IsItemEnabled(index ) )
				return;	// don't click there
		}
		CWnd::OnLButtonUp( nFlags, point );
	}

}

