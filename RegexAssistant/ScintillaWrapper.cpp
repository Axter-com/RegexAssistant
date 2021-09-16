/*
	Copyright (C) 2021  David Maisonave
	The RegexAssistant source code is free software. You can redistribute it and/or modify it under the terms of the GNU General Public License.
	This program is distributed in the hope that it will be useful,	but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
*/
#include "pch.h"
#include "RegexAssistantDlg_enums.h"
#include "DlgProxy.h"
#include "afxdialogex.h"
#include "CommonFunctions.h"
#include "ScintillaWrapper.h"
#include <vector>
#include <string>
#include <memory>

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
	return (int)0;
}

CString ScintillaWrapper::TextToFind_to_CString( Sci_TextToFind& ft )
{
	if ( !IsInit() )
		return _T( "" );
	Sci_TextRange Txrg;
	Txrg.chrg.cpMin = ft.chrgText.cpMin;
	Txrg.chrg.cpMax = ft.chrgText.cpMax;
	size_t stCchBuffer = Txrg.chrg.cpMax - Txrg.chrg.cpMin + 2;
	std::unique_ptr<char> apBuffer( new char[stCchBuffer] );
	apBuffer.get()[0] = '\0';
	Txrg.lpstrText = apBuffer.get();
	SendEditor( SCI_GETTEXTRANGE, 0, reinterpret_cast<sptr_t>(&Txrg) );

	return FXString::ToWString( apBuffer.get() ).c_str();
}


bool ScintillaWrapper::SetPersistentSetings()
{
	if ( !IsInit() )
		return false;
	CString FontName = _T( "Courier New" );
	SendEditor( SCI_STYLESETFONT, STYLE_DEFAULT, reinterpret_cast<sptr_t>(FXString::ToString( (LPCTSTR)FontName ).c_str()) );
	SendEditor( SCI_STYLESETSIZE, STYLE_DEFAULT, 10 );
	SendEditor( SCI_STYLESETBACK, STYLE_DEFAULT, RGB( 255, 255, 255 ) );
	SendEditor( SCI_STYLESETFORE, STYLE_DEFAULT, RGB( 0, 0, 0 ) );
	SendEditor( SCI_SETSELFORE, true, RGB( 0, 0, 0 ) );
	SendEditor( SCI_SETSELBACK, true, RGB( 228, 246, 254 ) );
	SendEditor( SCI_STYLECLEARALL, 0, 0 );
	return true;
}

