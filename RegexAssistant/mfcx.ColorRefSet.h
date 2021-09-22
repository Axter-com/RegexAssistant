// Copyright (C) 2021  David Maisonave

#pragma once

#define GO_TO_FIRSTVALIDITEM	-16
#define GO_TO_LASTVALIDITEM		-32

namespace mfcx
{
	enum
	{
		DEFAULT_DISABLE_FG_COLOR = RGB( 0x80, 0x80, 0x80 ),
		DEFAULT_COLORS = RGB( 0, 1, 2 )
	};
	// Helper classes for ComboBox class
	class ColorRefSet
	{
	public:
		ColorRefSet( COLORREF EnabledColor = DEFAULT_COLORS, COLORREF EnabledBkColor = DEFAULT_COLORS )
			:DisabledColor( DEFAULT_COLORS ), DisabledBkColor( DEFAULT_COLORS )
			, EnabledColor( EnabledColor ), EnabledBkColor( EnabledBkColor )
			, EnabledSelectColor( DEFAULT_COLORS ), EnabledSelectBkColor( DEFAULT_COLORS )
		{

		}
		ColorRefSet( COLORREF DisabledColor, COLORREF DisabledBkColor, COLORREF EnabledColor, COLORREF EnabledBkColor = DEFAULT_COLORS,
					 COLORREF EnabledSelectColor = DEFAULT_COLORS, COLORREF EnabledSelectBkColor = DEFAULT_COLORS )
			:DisabledColor( DisabledColor ), DisabledBkColor( DisabledBkColor )
			, EnabledColor( EnabledColor ), EnabledBkColor( EnabledBkColor )
			, EnabledSelectColor( EnabledSelectColor ), EnabledSelectBkColor( EnabledSelectBkColor )
		{

		}
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
					 COLORREF EnabledSelectColor = DEFAULT_COLORS, COLORREF EnabledSelectBkColor = DEFAULT_COLORS )
			:ColorRefSet( DisabledColor, DisabledBkColor, EnabledColor, EnabledBkColor, EnabledSelectColor, EnabledSelectBkColor )
		{
		}
	};

	class TextColorRefSet : public ColorRefSet //Change only Text colors, and use default background colors
	{
	public:
		TextColorRefSet( COLORREF DisabledColor, COLORREF EnabledColor = DEFAULT_COLORS, COLORREF EnabledSelectColor = DEFAULT_COLORS )
			: ColorRefSet( DisabledColor, DEFAULT_COLORS, EnabledColor, DEFAULT_COLORS, EnabledSelectColor, DEFAULT_COLORS )
		{

		}
	};

	class TextBackGroundColorRefSet : public ColorRefSet //Change only text back ground colors, and use default text colors
	{
	public:
		TextBackGroundColorRefSet( COLORREF DisabledBkColor, COLORREF EnabledBkColor = DEFAULT_COLORS, COLORREF EnabledSelectBkColor = DEFAULT_COLORS )
			: ColorRefSet( DEFAULT_COLORS, DisabledBkColor, DEFAULT_COLORS, EnabledBkColor, DEFAULT_COLORS, EnabledSelectBkColor )
		{
		}
	};

	class ComboBoxItemDetails : public ColorRefSet
	{
	public:
		ComboBoxItemDetails( BOOL isenabled = TRUE, CString tooltipstr = _T( "" ),
							 COLORREF DisabledColor = DEFAULT_COLORS, COLORREF DisabledBkColor = DEFAULT_COLORS,
							 COLORREF EnabledColor = DEFAULT_COLORS, COLORREF EnabledBkColor = DEFAULT_COLORS,
							 COLORREF EnabledSelectColor = DEFAULT_COLORS, COLORREF EnabledSelectBkColor = DEFAULT_COLORS )
			:IsEnabled( isenabled ), ToolTipStr( tooltipstr )
			, ColorRefSet( DisabledColor, DisabledBkColor, EnabledColor, EnabledBkColor, EnabledSelectColor, EnabledSelectBkColor )
		{
		}

		BOOL IsEnabled;
		CString ToolTipStr;
	};

	/////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////
	// ComboBoxListBox functions
	class ComboBox;
	class ComboBoxListBox : public CWnd
	{
	public:
		ComboBoxListBox() :m_Parent( NULL ) {}
		void SetParent( ComboBox * ptr ) { m_Parent = ptr; }
		//{{AFX_VIRTUAL(ComboBoxListBox)
		//}}AFX_VIRTUAL
	protected:
		ComboBox *m_Parent;
		//{{AFX_MSG(ComboBoxListBox)
		afx_msg void OnLButtonUp( UINT nFlags, CPoint point );
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP()
	};

}
