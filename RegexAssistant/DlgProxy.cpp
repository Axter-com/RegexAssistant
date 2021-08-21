
// DlgProxy.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "RegexAssistant.h"
#include "DlgProxy.h"
#include "RegexAssistantDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CRegexAssistantDlgAutoProxy

IMPLEMENT_DYNCREATE(CRegexAssistantDlgAutoProxy, CCmdTarget)

CRegexAssistantDlgAutoProxy::CRegexAssistantDlgAutoProxy()
{
	EnableAutomation();

	// To keep the application running as long as an automation
	//	object is active, the constructor calls AfxOleLockApp.
	AfxOleLockApp();

	// Get access to the dialog through the application's
	//  main window pointer.  Set the proxy's internal pointer
	//  to point to the dialog, and set the dialog's back pointer to
	//  this proxy.
	ASSERT_VALID(AfxGetApp()->m_pMainWnd);
	if (AfxGetApp()->m_pMainWnd)
	{
		ASSERT_KINDOF(CRegexAssistantDlg, AfxGetApp()->m_pMainWnd);
		if (AfxGetApp()->m_pMainWnd->IsKindOf(RUNTIME_CLASS(CRegexAssistantDlg)))
		{
			m_pDialog = reinterpret_cast<CRegexAssistantDlg*>(AfxGetApp()->m_pMainWnd);
			m_pDialog->m_pAutoProxy = this;
		}
	}
}

CRegexAssistantDlgAutoProxy::~CRegexAssistantDlgAutoProxy()
{
	// To terminate the application when all objects created with
	// 	with automation, the destructor calls AfxOleUnlockApp.
	//  Among other things, this will destroy the main dialog
	if (m_pDialog != nullptr)
		m_pDialog->m_pAutoProxy = nullptr;
	AfxOleUnlockApp();
}

void CRegexAssistantDlgAutoProxy::OnFinalRelease()
{
	// When the last reference for an automation object is released
	// OnFinalRelease is called.  The base class will automatically
	// deletes the object.  Add additional cleanup required for your
	// object before calling the base class.

	CCmdTarget::OnFinalRelease();
}

BEGIN_MESSAGE_MAP(CRegexAssistantDlgAutoProxy, CCmdTarget)
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CRegexAssistantDlgAutoProxy, CCmdTarget)
END_DISPATCH_MAP()

// Note: we add support for IID_IRegexAssistant to support typesafe binding
//  from VBA.  This IID must match the GUID that is attached to the
//  dispinterface in the .IDL file.

// {1b0815f4-8a77-436b-b931-ed3d53ff6a48}
static const IID IID_IRegexAssistant =
{0x1b0815f4,0x8a77,0x436b,{0xb9,0x31,0xed,0x3d,0x53,0xff,0x6a,0x48}};

BEGIN_INTERFACE_MAP(CRegexAssistantDlgAutoProxy, CCmdTarget)
	INTERFACE_PART(CRegexAssistantDlgAutoProxy, IID_IRegexAssistant, Dispatch)
END_INTERFACE_MAP()

// The IMPLEMENT_OLECREATE2 macro is defined in pch.h of this project
// {4422cfd8-7470-47ad-ae16-4b1fd8219fff}
IMPLEMENT_OLECREATE2(CRegexAssistantDlgAutoProxy, "RegexAssistant.Application", 0x4422cfd8,0x7470,0x47ad,0xae,0x16,0x4b,0x1f,0xd8,0x21,0x9f,0xff)


// CRegexAssistantDlgAutoProxy message handlers
