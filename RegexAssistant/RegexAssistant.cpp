/*
	Copyright (C) 2021  David Maisonave
	The RegexAssistant source code is free software. You can redistribute it and/or modify it under the terms of the GNU General Public License.
	This program is distributed in the hope that it will be useful,	but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
*/
#include "pch.h"
#include "framework.h"
#include "RegexAssistant.h"
#include "RegexAssistantDlg.h"
#include "cxxopts.hpp"
#include <string>
#include <vector>
#include "CommonFunctions.h"
#include <stdlib.h>

using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CRegexAssistantApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()
CRegexAssistantApp::CRegexAssistantApp()
{
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;
}

CRegexAssistantApp theApp;
const GUID CDECL BASED_CODE _tlid =	{0x99709d1c,0x796f,0x42cf,{0x8a,0x28,0x0d,0x69,0x10,0x11,0xd5,0x57}};
const WORD _wVerMajor = 1;
const WORD _wVerMinor = 0;

BOOL CRegexAssistantApp::InitInstance()
{
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);
	CWinApp::InitInstance();
	AfxEnableControlContainer();
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}

	AfxEnableControlContainer();
	CShellManager *pShellManager = new CShellManager;
	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));
	CCommandLineInfo cmdInfo;
	ParseCommandLine( cmdInfo );
	if ( cmdInfo.m_bRunEmbedded || cmdInfo.m_bRunAutomated )
	{
		COleTemplateServer::RegisterAll();
	} else if ( cmdInfo.m_nShellCommand == CCommandLineInfo::AppUnregister )
	{
		COleObjectFactory::UpdateRegistryAll( FALSE );
		AfxOleUnregisterTypeLib( _tlid, _wVerMajor, _wVerMinor );
		return FALSE;
	} else
	{
		COleObjectFactory::UpdateRegistryAll();
		AfxOleRegisterTypeLib( AfxGetInstanceHandle(), _tlid );
		if ( cmdInfo.m_nShellCommand == CCommandLineInfo::AppRegister )
			return FALSE;
	}

	cxxopts::Options options( "RegexAssistant", "**********  Regular Expression (Regex) Assistant  **********" );
	options.allow_unrecognised_options()
		.add_options()
		("d,debug", "Enable debugging", cxxopts::value<bool>()->default_value( "false" )) // ToDo: Delete this argument if it doesn't get used.
		("v,verbose", "Verbose output", cxxopts::value<bool>()->default_value( "false" ))
		("h,help", "Print usage")
		("m,monitor_no", "Monitor to start on", cxxopts::value<int>()->default_value("0"), "N")
		("x,regex", "Regular Expression (Regex) to populate field", cxxopts::value<string>()->default_value( "foo\\S*" ))
		("r,regex_replace", "Name of language to export", cxxopts::value<string>()->default_value( "" ))
		("s,sample_text", "Text to populated sample field", cxxopts::value<string>()->default_value( "" ))
		("f,file", "Name of file to get content for sample field", cxxopts::value<string>()->default_value( "" ))
		("c,clipboard", "Populate sample field with content from clipboard", cxxopts::value<bool>()->default_value( "false" ), "bool")
		("t,compatibility", "Regex compatibility target.", cxxopts::value<string>()->default_value( "" ))
		;
	auto arg_result = options.parse( __argc, __wargv );
	
	if ( arg_result.count( "help" ) )
	{
		AfxMessageBox( Common::ToWString( options.help() ).c_str() );
		exit( 0 );
	}

	CString regex_search = Common::ToWString(arg_result["regex"].as<string>()).c_str();
	CString regex_replace = Common::ToWString( arg_result["regex_replace"].as<string>() ).c_str();
	CString Sample = Common::ToWString( arg_result["sample_text"].as<string>() ).c_str() ;
	SampleLoadMethod sampleloadmethod = SampleLoadDefault;
	int MonitorToDisplay = arg_result["monitor_no"].as<int>();
	CString SampleFileName = Common::ToWString( arg_result["file"].as<string>() ).c_str();
	bool ClipboardSampleLoad = arg_result["clipboard"].as<bool>();
	Regex_Compatibility regex_compatibility = REGEX_COMPATIBILITY_SCINTILLA_POSIX;

	if ( ClipboardSampleLoad )
		sampleloadmethod = SampleLoadMethod::SampleLoadFromClipboard;
	else if ( SampleFileName.GetLength() > 0 )
	{
		Sample = SampleFileName;
		sampleloadmethod = SampleLoadMethod::SampleLoadFromFile;
	}
	else if ( Sample.GetLength() > 0 )
	{
		sampleloadmethod = SampleLoadMethod::SampleLoadFromCommandLine;
	}

	CRegexAssistantDlg dlg( regex_search , regex_replace, Sample, sampleloadmethod, MonitorToDisplay, regex_compatibility );
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)	{
	}
	else if (nResponse == IDCANCEL)	{
	}
	else if (nResponse == -1)	{
		TRACE(traceAppMsg, 0, "Warning: dialog creation failed, so application is terminating unexpectedly.\n");
		TRACE(traceAppMsg, 0, "Warning: if you are using MFC controls on the dialog, you cannot #define _AFX_NO_MFC_CONTROLS_IN_DIALOGS.\n");
	}

	if (pShellManager != nullptr)
	{
		delete pShellManager;
	}

#if !defined(_AFXDLL) && !defined(_AFX_NO_MFC_CONTROLS_IN_DIALOGS)
	ControlBarCleanUp();
#endif
	return FALSE;
}

int CRegexAssistantApp::ExitInstance()
{
	AfxOleTerm(FALSE);
	return CWinApp::ExitInstance();
}
