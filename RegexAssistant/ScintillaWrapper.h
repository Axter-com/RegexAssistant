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
	CString TextToFind_to_CString( TextToFind& ft );
	inline HWND GetWnd() { return m_hwndScintilla; }
	void SetExtContextMenuHelper( Sci_ExtContextMenuHelper * sci_extcontextmenuhelper );

protected:
	friend class CRegexAssistantDlg;
private:
	HWND m_hwndScintilla;
	SciFnDirect m_ScintillaFunction;
	sptr_t m_ScintillaInst;
};

