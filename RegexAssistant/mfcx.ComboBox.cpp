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

namespace mfcx
{
	/////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////
	// ComboBoxItemDetails class and implementation
	class ComboBoxItemDetails
	{
	public:
		ComboBoxItemDetails( BOOL isenabled, CString tooltipstr = _T( "" ) )
			:IsEnabled( isenabled ), ToolTipStr( tooltipstr ){}
		BOOL IsEnabled;
		CString ToolTipStr;
	};

	/////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////
	// ComboBoxListBox functions
	class ComboBoxListBox : public CWnd
	{
	public:
		ComboBoxListBox() :m_Parent( NULL ) {}
		void SetParent( ComboBox * ptr){m_Parent = ptr;}
		//{{AFX_VIRTUAL(ComboBoxListBox)
		//}}AFX_VIRTUAL
	protected:
		ComboBox *m_Parent;
		//{{AFX_MSG(ComboBoxListBox)
		afx_msg void OnLButtonUp( UINT nFlags, CPoint point );
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP()
	};

	/////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////
	// ComboBox implementation
	const UINT nMessage = ::RegisterWindowMessage( _T( "ComboSelEndOK" ) );

	ComboBox::ComboBox( COLORREF DisabledColor, COLORREF DisabledBkColor, COLORREF AlternateDisabledColor )
		:m_ToolTipItemRecievedStr( FALSE ), m_bLastCB_Sel( -2 ), m_DisabledColor( DisabledColor )
		, m_DisabledBkColor( DisabledBkColor ), m_AlternateDisabledColor( AlternateDisabledColor )
		, m_ListBox( new ComboBoxListBox() ), m_DoToolTipPopupWhenItemStringTooWide(FALSE)
	{
		Initiate();
	}

	ComboBox::ComboBox( CString ToolTipString, COLORREF DisabledColor, COLORREF DisabledBkColor, COLORREF AlternateDisabledColor )
		:m_ToolTipItemRecievedStr( FALSE ), m_bLastCB_Sel( -2 ), m_ToolTipString( ToolTipString ), m_DisabledColor( DisabledColor )
		, m_DisabledBkColor( DisabledBkColor ), m_AlternateDisabledColor( AlternateDisabledColor )
		, m_ListBox( new ComboBoxListBox() ), m_DoToolTipPopupWhenItemStringTooWide( FALSE )
	{
		Initiate();
	}

	void ComboBox::Initiate()
	{
		m_ListBox->SetParent( this );
		NONCLIENTMETRICS ncm = {sizeof( NONCLIENTMETRICS )};
		SystemParametersInfo( SPI_GETNONCLIENTMETRICS, sizeof( NONCLIENTMETRICS ), &ncm, 0 );
		m_font.CreateFontIndirect( &ncm.lfStatusFont );

	}

	ComboBox::~ComboBox()
	{
	}

	BEGIN_MESSAGE_MAP( ComboBox, CComboBox )
		//{{AFX_MSG_MAP(ComboBox)
		ON_WM_CHARTOITEM()
		ON_CONTROL_REFLECT( CBN_SELENDOK, OnSelendok )
		ON_WM_MOUSEMOVE()
		ON_WM_MOUSEWHEEL()
		ON_MESSAGE( WM_CTLCOLORLISTBOX, OnCtlColor )
		ON_REGISTERED_MESSAGE( nMessage, OnRealSelEndOK )
		//}}AFX_MSG_MAP
	END_MESSAGE_MAP()

	void ComboBox::PreSubclassWindow()
	{
		_ASSERT_EXPR(( (GetStyle()&(CBS_OWNERDRAWFIXED | CBS_HASSTRINGS)) == (CBS_OWNERDRAWFIXED | CBS_HASSTRINGS)), 
		 _T("This ComboBox control requires CBS_OWNERDRAWFIXED and CBS_HASSTRINGS.  Look at this ComboBox's properties settings under [Behavior], and set [Has String] to 'true' and set [Owner Draw] to 'Fixed'" ));
		UpdateMainTooltip( m_ToolTipString );

		m_hwndItemTip.CreateEx( WS_EX_TOPMOST, TOOLTIPS_CLASS, NULL,
								WS_POPUP | TTS_ALWAYSTIP | TTS_BALLOON,
								CW_USEDEFAULT, CW_USEDEFAULT,
								CW_USEDEFAULT, CW_USEDEFAULT,
								m_ListBox->GetSafeHwnd(), NULL );

		CComboBox::PreSubclassWindow();
	}

	void ComboBox::DrawItem( LPDRAWITEMSTRUCT lpDrawItemStruct )
	{
		CDC* pDC = CDC::FromHandle( lpDrawItemStruct->hDC );

		if ( ((LONG)(lpDrawItemStruct->itemID) >= 0) &&
			(lpDrawItemStruct->itemAction & (ODA_DRAWENTIRE | ODA_SELECT)) )
		{
			CString ToolTipItem;
			BOOL fDisabled = !IsWindowEnabled() || !IsItemEnabled( lpDrawItemStruct->itemID, &ToolTipItem );
			COLORREF newTextColor = fDisabled ? m_DisabledColor : GetSysColor( COLOR_WINDOWTEXT );  // light gray
			COLORREF newBkColor = (fDisabled && m_DisabledBkColor != USE_DEFAULT_WINDOWS_BK_COLOR && m_DisabledBkColor != m_DisabledColor) ? m_DisabledBkColor : GetSysColor( COLOR_WINDOW );

			if ( newTextColor == newBkColor )
				newTextColor = m_AlternateDisabledColor;   // dark gray

			COLORREF oldTextColor = pDC->SetTextColor( newTextColor );
			COLORREF oldBkColor = pDC->SetBkColor( newBkColor );

			if ( !fDisabled && ((lpDrawItemStruct->itemState & ODS_SELECTED) != 0) )
			{
				pDC->SetTextColor( GetSysColor( COLOR_HIGHLIGHTTEXT ) );
				pDC->SetBkColor( GetSysColor( COLOR_HIGHLIGHT ) );
			}

			CFont* pOldFont = pDC->SelectObject( &m_font );
			CString strText;
			GetLBText( lpDrawItemStruct->itemID, strText );
			const RECT &rc = lpDrawItemStruct->rcItem;
			pDC->ExtTextOut( rc.left + 2,
							 rc.top + 2,// + max(0, (cyItem - m_cyText) / 2),
							 ETO_OPAQUE, &rc,
							 strText, NULL );
			CSize TextSize = pDC->GetTextExtent( strText );

			if ( m_DoToolTipPopupWhenItemStringTooWide && (TextSize.cx > (lpDrawItemStruct->rcItem.right - lpDrawItemStruct->rcItem.left))  )
			{
				if ( ToolTipItem.IsEmpty() )
					ToolTipItem = strText;
				else
					ToolTipItem = _T( "[" ) + strText + _T( "] -- " ) + ToolTipItem;
			}
			m_ToolTipItemRecievedStr = (ToolTipItem.IsEmpty()) ? m_ToolTipItemRecievedStr : TRUE;

			if ( m_hwndItemTip.m_hWnd != NULL && m_ToolTipItemRecievedStr )
			{
				CRect rectClient( rc );
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
					toolInfo.lpszText = const_cast<WCHAR*>(ToolTipItem.operator LPCWSTR());
					rectClient.left += 1;
					rectClient.top += 3;
					m_hwndItemTip.SendMessage( TTM_TRACKPOSITION, 0, static_cast<LPARAM>(MAKELONG( rectClient.left, rectClient.top )) );
					m_hwndItemTip.SendMessage( TTM_UPDATETIPTEXT, 0, reinterpret_cast<LPARAM>(&toolInfo) );
					m_hwndItemTip.SendMessage( TTM_ADDTOOL, 0, reinterpret_cast<LPARAM>(&toolInfo) );
				}
			}

			pDC->SelectObject( pOldFont );
			pDC->SetTextColor( oldTextColor );
			pDC->SetBkColor( oldBkColor );
		} 
		else if ( (LONG)(lpDrawItemStruct->itemID) < 0 )	// drawing edit text
		{
			COLORREF newTextColor = GetSysColor( COLOR_WINDOWTEXT );  // light gray
			COLORREF oldTextColor = pDC->SetTextColor( newTextColor );

			COLORREF newBkColor = GetSysColor( COLOR_WINDOW );
			COLORREF oldBkColor = pDC->SetBkColor( newBkColor );

			if ( (lpDrawItemStruct->itemState & ODS_SELECTED) != 0 )
			{
				pDC->SetTextColor( GetSysColor( COLOR_HIGHLIGHTTEXT ) );
				pDC->SetBkColor( GetSysColor( COLOR_HIGHLIGHT ) );
			}

			CString strText;
			GetWindowText( strText );
			const RECT &rc = lpDrawItemStruct->rcItem;

			pDC->ExtTextOut( rc.left + 2,
							 rc.top + 2,// + max(0, (cyItem - m_cyText) / 2),
							 ETO_OPAQUE, &rc,
							 strText, strText.GetLength(), NULL );

			pDC->SetTextColor( oldTextColor );
			pDC->SetBkColor( oldBkColor );
		}
		if ( (lpDrawItemStruct->itemAction & ODA_FOCUS) != 0 )
			pDC->DrawFocusRect( &lpDrawItemStruct->rcItem );
	}

	void ComboBox::MeasureItem( LPMEASUREITEMSTRUCT lpMeasureItemStruct )
	{
		//Add code to determine the size of specified item
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
		GetWindowText( m_strSavedText );
		PostMessage( nMessage );
	}

	LRESULT ComboBox::OnRealSelEndOK( WPARAM, LPARAM )
	{
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
		//	m_ListBox->UnsubclassWindow();
		m_ListBox->Detach();
		CComboBox::PostNcDestroy();
	}

	BOOL ComboBox::IsItemEnabled( UINT nIndex, CString * ToolTipFetch ) const
	{
		if ( nIndex >= static_cast<DWORD>(GetCount()) )
			return TRUE;

		ComboBoxItemDetails* data = static_cast<ComboBoxItemDetails*>(GetItemDataPtr( nIndex ));
		if ( data == NULL )
			return TRUE;
		if ( ToolTipFetch != NULL )
			*ToolTipFetch = data->ToolTipStr;
		return data->IsEnabled;
	}

	void ComboBox::SetItemDetails( UINT nIndex, BOOL IsEnabled, CString ToolTipStr )
	{
		if ( nIndex >= static_cast<DWORD>(GetCount()) )
			return;

		ComboBoxItemDetails* data = new ComboBoxItemDetails( IsEnabled, ToolTipStr );
		m_vComboBoxItemDetails.push_back( std::shared_ptr<ComboBoxItemDetails>( data ) );
		SetItemDataPtr( nIndex, static_cast<void*>(data) );
	}

	int ComboBox::AddString( const CString &str, BOOL IsEnabled, CString ToolTipStr )
	{
		int nIndex = CComboBox::AddString( str );
		if ( nIndex != CB_ERR )
			SetItemDetails( nIndex, IsEnabled, ToolTipStr );

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
			if ( !outside && !m_Parent->IsItemEnabled( index ) )
				return;	// don't click there
		}
		CWnd::OnLButtonUp( nFlags, point );
	}


}