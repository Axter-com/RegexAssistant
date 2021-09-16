/*
	Copyright (C) 2021  David Maisonave
	The RegexAssistant source code is free software. You can redistribute it and/or modify it under the terms of the GNU General Public License.
	This program is distributed in the hope that it will be useful,	but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
*/
#pragma once

#include "framework.h"
#include "RegexAssistant.h"
#include "include/Scintilla.h"
class Sci_ExtContextMenuHelper;

class ScintillaWrapper
{
public:
	ScintillaWrapper( HWND hwndScintilla = NULL );
	~ScintillaWrapper();
	bool Init( HWND hwndScintilla );
	int SendEditor( UINT Msg, WPARAM wParam = 0, LPARAM lParam = 0 );
	inline bool IsInit() { return (m_ScintillaFunction != NULL); }
	bool SetPersistentSetings();
	CString TextToFind_to_CString( Sci_TextToFind& ft );
	inline HWND GetWnd() { return m_hwndScintilla; }

protected:
	friend class CRegexAssistantDlg;
private:
	HWND m_hwndScintilla;
	SciFnDirect m_ScintillaFunction;
	sptr_t m_ScintillaInst;
};

