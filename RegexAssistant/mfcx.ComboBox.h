/*
	Copyright (C) 2021  David Maisonave
	The mfcx source code is free software. You can redistribute it and/or modify it under the terms of the GNU General Public License.
	This program is distributed in the hope that it will be useful,	but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

	Purpose:
		Add tooltip, disable items, and custom color functionality to CComboBox class.
			1. Includes implimentation that adds tooltips for each item in the ComboBox list.
			2. Implimentation for adding a tooltip to the main ComboBox window.
			3. Can disable specific items in the ComboBox.
			4. Can set disabled text color and background color.
			5. Able to set custom colors for all items.
			6. Able to set a different custom colors for each item on the list.
			7. Able to set option to produce a tooltip popup window only when string is too wide to fit in the ComboBox.

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
	enum
	{
		DEFAULT_DISABLE_FG_COLOR = RGB( 0x80, 0x80, 0x80 ),
		DEFAULT_COLORS = RGB( 0, 1, 2 )
	};

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Helper classes for ComboBox class
	class ColorRefSet
	{
	public:
		ColorRefSet( COLORREF EnabledColor = DEFAULT_COLORS, COLORREF EnabledBkColor = DEFAULT_COLORS );
		ColorRefSet( COLORREF DisabledColor, COLORREF DisabledBkColor, COLORREF EnabledColor, COLORREF EnabledBkColor = DEFAULT_COLORS,
					 COLORREF EnabledSelectColor = DEFAULT_COLORS, COLORREF EnabledSelectBkColor = DEFAULT_COLORS );
		COLORREF DisabledColor;
		COLORREF DisabledBkColor;
		COLORREF EnabledColor;
		COLORREF EnabledBkColor;
		COLORREF EnabledSelectColor;
		COLORREF EnabledSelectBkColor;
	};

	class DisableColorRefSet : public ColorRefSet
	{
	public:
		DisableColorRefSet( COLORREF DisabledColor, COLORREF DisabledBkColor,
					 COLORREF EnabledColor = DEFAULT_COLORS, COLORREF EnabledBkColor = DEFAULT_COLORS,
					 COLORREF EnabledSelectColor = DEFAULT_COLORS, COLORREF EnabledSelectBkColor = DEFAULT_COLORS );
	};

	class TextColorRefSet : public ColorRefSet //Change only Text colors, and use default background colors
	{
	public:
		TextColorRefSet( COLORREF DisabledColor, COLORREF EnabledColor = DEFAULT_COLORS, COLORREF EnabledSelectColor = DEFAULT_COLORS );
	};

	class TextBackGroundColorRefSet : public ColorRefSet //Change only text back ground colors, and use default text colors
	{
	public:
		TextBackGroundColorRefSet( COLORREF DisabledBkColor, COLORREF EnabledBkColor = DEFAULT_COLORS, COLORREF EnabledSelectBkColor = DEFAULT_COLORS );
	};

	class ComboBoxListBox;
	class ComboBoxItemDetails;
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// ComboBox class (The main class) -- See top of header file for usage details.
	class ComboBox : public CComboBox
	{
		static const ColorRefSet DefaultColors;
		static const TextColorRefSet DefaultTextColors;
		static const TextBackGroundColorRefSet DefaultTextBkColors;
	public:
		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		////  ------------------------------------------------------------------------------------------------------------------------------------
		////  Use the AddString function to optionally set per list item's enabled status, tooltip string, and custom color					//////
		/**/  virtual int AddString( const CString &str, BOOL IsEnabled = TRUE, const CString &ToolTipStr = _T( "" ),						//////
		/**/									const ColorRefSet &colorrefset = DefaultColors );											//////
		/**/									// colorrefset can take a type from any of the derived classes (DisableColorRefSet,			//////
		/**/									// TextColorRefSet, and TextBackGroundColorRefSet), as well as type ColorRefSet				//////
		////  ------------------------------------------------------------------------------------------------------------------------------------
		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		/////////////////////////////////////////////////////////////////////////////
		// For custom behaviour, override the following virtual functions
		virtual void EnableWideStrPopup( BOOL Enabled = TRUE );
		virtual BOOL UpdateMainTooltip( const CString &value );

		ComboBox( const ColorRefSet &colorrefset = DefaultColors );

		ComboBox( const CString &ToolTipString, const ColorRefSet &colorrefset = DefaultColors );

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
		const COLORREF m_EnabledColor;
		const COLORREF m_EnabledBkColor;
		const COLORREF m_EnabledSelectColor;
		const COLORREF m_EnabledSelectBkColor;
		std::vector<std::shared_ptr<ComboBoxItemDetails> > m_vComboBoxItemDetails;
		CFont m_font;
	protected:
		CString m_strSavedText;	// saves text between OnSelendok and OnRealSelEndOK calls
		std::unique_ptr<ComboBoxListBox> m_ListBox;
		void Initiate();
		virtual COLORREF GetDisabledItemTextColor( BOOL IsDisabled, BOOL IsItemSelected, const TextColorRefSet &Superseded = DefaultTextColors );
		virtual COLORREF GetDisabledItemTextBkColor( BOOL IsDisabled, BOOL IsItemSelected, const TextBackGroundColorRefSet &Superseded = DefaultTextBkColors );
		void SetItemDetails( UINT nIndex, BOOL IsEnabled, CString ToolTipStr = _T( "" ), const ColorRefSet &colorrefset = DefaultColors ); // Called by AddString and sets data via SetItemDataPtr
		//{{AFX_VIRTUAL(ComboBox)
		virtual void DrawItem( LPDRAWITEMSTRUCT lpDrawItemStruct );
		virtual void MeasureItem( LPMEASUREITEMSTRUCT lpMeasureItemStruct );
		virtual void PreSubclassWindow();
		virtual void PostNcDestroy();
		//}}AFX_VIRTUAL

		//{{AFX_MSG(ComboBox)
		afx_msg int OnCharToItem( UINT nChar, CListBox* pListBox, UINT nIndex );
		afx_msg void OnSelendok();
		afx_msg LRESULT OnRealSelEndOK( WPARAM, LPARAM );
		//}}AFX_MSG
		LRESULT OnCtlColor( WPARAM, LPARAM lParam );
		DECLARE_MESSAGE_MAP()
	};
}