/*
	Copyright (C) 2021  David Maisonave
	The RegexAssistant source code is free software. You can redistribute it and/or modify it under the terms of the GNU General Public License.
	This program is distributed in the hope that it will be useful,	but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
*/
#pragma once

#include "framework.h"
#include "include/Scintilla.h"
class Sci_ExtContextMenuHelper;

class ScintillaWrapper
{
public:
	enum Sci_GetAction {Sci_GetText, Sci_GetLine};
	ScintillaWrapper( HWND hwndScintilla = NULL );
	~ScintillaWrapper();
	inline bool IsInit() { return (m_ScintillaFunction != NULL); }
	inline HWND GetWnd() { return m_hwndScintilla; }
	bool Init( HWND hwndScintilla );
	int SendEditor( UINT Msg, WPARAM wParam = 0, LPARAM lParam = 0 );
	int SendEditor( UINT Msg, WPARAM value1, const std::string &value2 );
	int SendEditor( UINT Msg, WPARAM value1, const CString &value2 );
	int SendEditor( UINT Msg, const std::string &value );
	int SendEditor( UINT Msg, const CString &value );
	int GetText( size_t Size, std::unique_ptr<char[]> &pcBuffer, Sci_GetAction sci_getaction = Sci_GetText );
	int SetStartStyling( const Sci_TextToFind &ft, int value );

	//int GetText( size_t Size, const char* Value);

protected:
	friend class CRegexAssistantDlg;
private:
	HWND m_hwndScintilla;
	SciFnDirect m_ScintillaFunction;
	sptr_t m_ScintillaInst;
};

