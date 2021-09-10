/*
	Copyright (C) 2021  David Maisonave
	The mfcx source code is free software. You can redistribute it and/or modify it under the terms of the GNU General Public License.
	This program is distributed in the hope that it will be useful,	but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

	Purpose:
		Add tooltip and disable items functionality to CComboBox class.
			1. Implimentation that adds tooltips for each item in the ComboBox list.
			2. Implimentation for adding a tooltip to the main ComboBox window.
			3. Can disable specific items in the ComboBox
			4. Can select disabled text color and background color
			5. Optionally when set, can produce a tooltip popup window only when string is too long to fit in the ComboBox.
	
	Usage Details:
		Important!!!: Set  properties settings to [Has String] = 'true' and [Owner Draw] = 'Fixed'
		Replace the CComboBox control with mfcx::ComboBox in the header.
		Use AddString in mfcx::ComboBox to add an item and set item's enabled status and optional item's tooltip string
		Options:
			Optionally call ComboBox constructor with color options for disabled-color and disabled-background-color.
			Optionally call the other constructor with the tooltip string for the main CComboBox window.
*/

#include <memory>
#include <vector>

namespace mfcx
{
	class ComboBoxListBox;
	class ComboBoxItemDetails;
	class ComboBox : public CComboBox
	{
	public:
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////
		////  ---------------------------------------------------------------------------------------------------------
		////  Use this AddString to set the item's enabled status and tooltip string                             //////
		/**/  virtual int AddString(const CString &str, BOOL IsEnabled = TRUE, CString ToolTipStr = _T( "" ) );  //////
		////  ---------------------------------------------------------------------------------------------------------
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////


		/////////////////////////////////////////////////////////////////////////////
		// For custom behaviour, override the following virtual functions
		virtual void EnableWideStrPopup( BOOL Enabled = TRUE );
		virtual BOOL IsItemEnabled(UINT nIndex, CString *ToolTipFetch = NULL) const;  // Default implementation uses data from GetItemDataPtr
		virtual void SetItemDetails( UINT nIndex, BOOL IsEnabled, CString ToolTipStr = _T("") ); // Called by AddString and sets data via SetItemDataPtr
		virtual BOOL UpdateMainTooltip( const CString &value );

		enum {
			DEFAULT_DISABLE_FG_COLOR = RGB( 0x80, 0x80, 0x80 ), 
			USE_DEFAULT_WINDOWS_BK_COLOR = RGB( 0, 1, 2 ), 
			ALTERNATIVE_DISABLE_FG_COLOR = RGB( 0xC0, 0xC0, 0xC0 )
		};

		ComboBox( COLORREF DisabledColor, COLORREF DisabledBkColor = USE_DEFAULT_WINDOWS_BK_COLOR, COLORREF AlternateDisabledColor = ALTERNATIVE_DISABLE_FG_COLOR );

		ComboBox( CString ToolTipString = _T(""), 
						   COLORREF DisabledColor = DEFAULT_DISABLE_FG_COLOR, 
						   COLORREF DisabledBkColor = USE_DEFAULT_WINDOWS_BK_COLOR, 
						   COLORREF AlternateDisabledColor = ALTERNATIVE_DISABLE_FG_COLOR );

		virtual ~ComboBox();
	private:
		CWnd  m_hwndTip;
		CWnd  m_hwndItemTip;
		int   m_bLastCB_Sel;
		BOOL  m_ToolTipItemRecievedStr;
		BOOL  m_DoToolTipPopupWhenItemStringTooWide;
		CString m_ToolTipString;
		const COLORREF m_DisabledColor;
		const COLORREF m_DisabledBkColor;
		const COLORREF m_AlternateDisabledColor;
		std::vector<std::shared_ptr<ComboBoxItemDetails> > m_vComboBoxItemDetails;
		CFont m_font;
	protected:
		CString m_strSavedText;	// saves text between OnSelendok and OnRealSelEndOK calls
		std::unique_ptr<ComboBoxListBox> m_ListBox;
		void Initiate();
		//{{AFX_VIRTUAL(ComboBox)
		virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
		virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
		virtual void PreSubclassWindow();
		virtual void PostNcDestroy();
		//}}AFX_VIRTUAL

		//{{AFX_MSG(ComboBox)
		afx_msg int OnCharToItem(UINT nChar, CListBox* pListBox, UINT nIndex);
		afx_msg void OnSelendok();
		afx_msg LRESULT OnRealSelEndOK( WPARAM, LPARAM );
		//}}AFX_MSG
		LRESULT OnCtlColor(WPARAM,LPARAM lParam);
		DECLARE_MESSAGE_MAP()
	};
}