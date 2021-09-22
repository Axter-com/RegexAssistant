/*
	Copyright (C) 2021  David Maisonave
	The RegexAssistant source code is free software. You can redistribute it and/or modify it under the terms of the GNU General Public License.
	This program is distributed in the hope that it will be useful,	but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
*/
#include "pch.h"
#include "CommonFunctions.h"
#include "ScintillaWrapper.h"
#include "RegexAssistant.h"
#include <string>

ScintillaWrapper::ScintillaWrapper( HWND hwndScintilla ) :m_hwndScintilla( NULL ), m_ScintillaInst( NULL ), m_ScintillaFunction( NULL )
{
	const int ScintillaRegClassResults = Scintilla_RegisterClasses( AfxGetInstanceHandle() );
	if ( ScintillaRegClassResults != 1 )
		REPORT_ERR_AND_EXIT( -3, "Failed to register Scintilla class.  Error no [%i]", ScintillaRegClassResults );
	Init( hwndScintilla );
}

ScintillaWrapper::~ScintillaWrapper()
{
	Scintilla_ReleaseResources();
}

bool ScintillaWrapper::Init( HWND hwndScintilla )
{
	if ( hwndScintilla == NULL )
		return false;

	m_hwndScintilla = hwndScintilla;
	m_ScintillaInst = (sptr_t)::SendMessage( m_hwndScintilla, SCI_GETDIRECTPOINTER, 0, 0 );
	if ( m_ScintillaInst == NULL )
	{
		REPORT_ERR_AND_EXIT( -1, "Could not get Scintilla pointer!" );
	}

	m_ScintillaFunction = (SciFnDirect)::SendMessage( m_hwndScintilla, SCI_GETDIRECTFUNCTION, 0, 0 );
	if ( m_ScintillaFunction == NULL )
	{
		REPORT_ERR_AND_EXIT( -2, "Could not get Scintilla function pointer!" );
	}

	std::string FontName = "Courier New";
	SendEditor( SCI_STYLESETFONT, STYLE_DEFAULT, reinterpret_cast<sptr_t>(FontName.c_str()) );
	SendEditor( SCI_STYLESETSIZE, STYLE_DEFAULT, 10 );
	SendEditor( SCI_STYLESETBACK, STYLE_DEFAULT, RGB( 255, 255, 255 ) );
	SendEditor( SCI_STYLESETFORE, STYLE_DEFAULT, RGB( 0, 0, 0 ) );
	SendEditor( SCI_SETSELFORE, true, RGB( 0, 0, 0 ) );
	SendEditor( SCI_SETSELBACK, true, RGB( 228, 246, 254 ) );
	SendEditor( SCI_STYLECLEARALL, 0, 0 );

	return true;
}

int ScintillaWrapper::SendEditor( UINT Msg, WPARAM wParam, LPARAM lParam )
{// Keep this for future context implementation
	static int dummyVar = 0;
	switch ( Msg )
	{
	case SCI_GETTEXTLENGTH:
	case SCI_GETCURRENTPOS:
	case SCI_LINEFROMPOSITION:
	case SCI_LINESONSCREEN:
	case SCI_GETLINECOUNT:
	case SCI_STYLEGETBACK:
	case SCI_FINDTEXT:
	case SCI_POSITIONFROMLINE:
	case SCI_GETLINEENDPOSITION:
	case SCI_STARTSTYLING:
	case SCI_SETSTYLING:
	case SCI_GETSELECTIONS:
	case SCI_GETSELECTIONEMPTY:
	case SCI_GETSELECTIONEND:
	case SCI_GETSELECTIONSTART:
	case SCI_GETLINE:
		break;
	default:
		++dummyVar;
	}

	if ( IsInit() )
		return (int)m_ScintillaFunction( m_ScintillaInst, Msg, wParam, lParam );
	return 0;
}

int ScintillaWrapper::SendEditor( UINT Msg, WPARAM value1, const std::string &value2 )
{
	return SendEditor( Msg, static_cast<WPARAM>(value1), reinterpret_cast<sptr_t>(value2.c_str()) );
}

int ScintillaWrapper::SendEditor( UINT Msg, WPARAM value1, const CString &value2 )
{
	return SendEditor( Msg, static_cast<WPARAM>(value1), reinterpret_cast<sptr_t>(Common::ToString( value2 ).c_str()) );
}

int ScintillaWrapper::SendEditor( UINT Msg, const std::string &value )
{
	return SendEditor( Msg, static_cast<WPARAM>(value.length()), reinterpret_cast<sptr_t>(value.c_str()) );
}

int ScintillaWrapper::SendEditor( UINT Msg, const CString &value )
{
	return SendEditor( Msg, static_cast<WPARAM>(value.GetLength()), reinterpret_cast<sptr_t>(Common::ToString( value ).c_str()) );
}

int ScintillaWrapper::SetStartStyling( const Sci_TextToFind &ft, int value )
{
	SendEditor( SCI_STARTSTYLING, ft.chrgText.cpMin, 0x1f );
	return SendEditor( SCI_SETSTYLING, ft.chrgText.cpMax - ft.chrgText.cpMin, value );
}
//
//int ScintillaWrapper::GetText( size_t Size, const char* Value )
//{
//	return SendEditor( SCI_GETTEXT, (uptr_t)Size, Value );
//}

int ScintillaWrapper::GetText( size_t Size, std::unique_ptr<char[]> &pcBuffer, Sci_GetAction sci_getaction )
{
	int RetrnVal = SendEditor( (sci_getaction == Sci_GetText ? SCI_GETTEXT : SCI_GETLINE), (uptr_t)Size, reinterpret_cast<sptr_t>(pcBuffer.get()) );
	if ( RetrnVal > 0 && RetrnVal < Size )
		pcBuffer[Size] = '\0';
	return RetrnVal;
}

