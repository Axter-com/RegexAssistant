
// DlgProxy.h: header file
//

#pragma once

class CRegexAssistantDlg;


// CRegexAssistantDlgAutoProxy command target

class CRegexAssistantDlgAutoProxy : public CCmdTarget
{
	DECLARE_DYNCREATE(CRegexAssistantDlgAutoProxy)

	CRegexAssistantDlgAutoProxy();           // protected constructor used by dynamic creation

// Attributes
public:
	CRegexAssistantDlg* m_pDialog;

// Operations
public:

// Overrides
	public:
	virtual void OnFinalRelease();

// Implementation
protected:
	virtual ~CRegexAssistantDlgAutoProxy();

	// Generated message map functions

	DECLARE_MESSAGE_MAP()
	DECLARE_OLECREATE(CRegexAssistantDlgAutoProxy)

	// Generated OLE dispatch map functions

	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()
};

