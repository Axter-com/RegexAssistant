/*
 * Author:
 *   HyeongCheol Kim <bluewiz96@gmail.com>
 *
 * Copyright (C) 2014 HyeongCheol Kim <bluewiz96@gmail.com>
 *
 * Released under GNU Lesser GPL, read the file 'COPYING' for more information
 */

#include "pch.h"
#include "MultiMonitors.h"
#include <vector>
#include <tchar.h>



class MultiMonitorsData
{
public:
	std::vector<MONITORINFOEX>	m_vMonitorInfo;
};

BOOL CALLBACK MultiMonitors::_MonitorEnumProc( 
	HMONITOR hMonitor,  // handle to display monitor
	HDC hdcMonitor,     // handle to monitor DC
	LPRECT lprcMonitor, // monitor intersection rectangle
	LPARAM dwData       // data
)
{
	MultiMonitors* pThis = reinterpret_cast<MultiMonitors*>(dwData);
	if ( pThis )
		pThis->AddMonitor( hMonitor, lprcMonitor );
	return pThis->MonitorEnumProc( hMonitor, hdcMonitor, lprcMonitor );
}

MultiMonitors::MultiMonitors() :m_pData( new MultiMonitorsData() )
, m_nPrimaryMonitorIndex( -1 )
, m_rcVirtualScrn( {0,0,0,0})
, m_EnumDisplayMonitorsSuccess(FALSE)
{
	::SetRectEmpty( &m_rcVirtualScrn );
	m_pData->m_vMonitorInfo.clear();

	m_EnumDisplayMonitorsSuccess = ::EnumDisplayMonitors( NULL, NULL, _MonitorEnumProc, reinterpret_cast<LPARAM>(this) );
}

MultiMonitors::~MultiMonitors()
{
	delete m_pData;
}

BOOL MultiMonitors::MonitorEnumProc( HMONITOR hMonitor, HDC /*hdcMonitor*/, LPRECT /*lprcMonitor*/ )
{
	MONITORINFOEX MonitorInfo{};
	MonitorInfo.cbSize = sizeof( MONITORINFOEX );

	if ( !::GetMonitorInfo( hMonitor, &MonitorInfo ) )
	{
		TRACE( _T("MultiMonitors::MonitorEnumProc - GetMonitorInfo Error: %d\r\n"), GetLastError() );
		return TRUE;
	}

	if ( m_pData->m_vMonitorInfo.size() == 0 )
	{
		m_rcVirtualScrn = MonitorInfo.rcMonitor;
	}
	else
	{
		::UnionRect( &m_rcVirtualScrn, &m_rcVirtualScrn, &MonitorInfo.rcMonitor );
	}

	m_pData->m_vMonitorInfo.push_back( MonitorInfo );

	if ( MonitorInfo.dwFlags & MONITORINFOF_PRIMARY )
		m_nPrimaryMonitorIndex = (int)( m_pData->m_vMonitorInfo.size()-1 );

	return TRUE;
}

UINT MultiMonitors::GetMonitorCount()
{
	return (UINT)m_pData->m_vMonitorInfo.size();
}

SIZE MultiMonitors::GetMonitorResolution( UINT nMonitor_Index )
{
	SIZE szRes{0,0};
	if ( GetMonitorCount() <= nMonitor_Index )
		return szRes;

	MONITORINFOEX& MonitorInfo = m_pData->m_vMonitorInfo[nMonitor_Index];
	szRes.cx = MonitorInfo.rcMonitor.right - MonitorInfo.rcMonitor.left;
	szRes.cy = MonitorInfo.rcMonitor.bottom - MonitorInfo.rcMonitor.top;
	return szRes;
}

RECT MultiMonitors::GetMonitorPosition( UINT nMonitor_Index )
{
	RECT rcPos{0,0,0,0};
	if ( GetMonitorCount() <= nMonitor_Index )
		return rcPos;

	MONITORINFOEX& MonitorInfo = m_pData->m_vMonitorInfo[nMonitor_Index];
	rcPos = MonitorInfo.rcMonitor;
	return rcPos;
}

UINT MultiMonitors::GetPrimaryMonitorIndex()
{
	return (m_nPrimaryMonitorIndex < 0) ? 0 : (UINT)m_nPrimaryMonitorIndex;
}


BOOL MultiMonitors::MoveWindowToMonitor( HWND hWnd, UINT nMonitor_Index)
{
	if ( GetMonitorCount() <= nMonitor_Index )
		return FALSE;

	RECT rect = {0};
	::GetWindowRect( hWnd, &rect );

	WINDOWPLACEMENT windowPlacement{};
	::GetWindowPlacement( hWnd, &windowPlacement );

	RECT curRect = windowPlacement.rcNormalPosition;
	RECT newRect = TransformRectToOtherScreen( curRect, nMonitor_Index );
	MoveWindow( hWnd, &windowPlacement, newRect );

	return TRUE;
}

RECT MultiMonitors::TransformRectToOtherScreen( const RECT& srcRect, int otherScreenIndex )
{
	// determine which screen this rectangle is (mainly) on
	HMONITOR hMonitor = MonitorFromRect( &srcRect, MONITOR_DEFAULTTONEAREST );

	int curScreenIndex = FindHMonitor( hMonitor );
	if ( curScreenIndex < 0 )
		return srcRect;

	return TransformRect( srcRect, curScreenIndex, otherScreenIndex );
}

RECT MultiMonitors::TransformRect( const RECT& srcRect, int srcScreenIndex, int destScreenIndex )
{
	RECT destRect = srcRect;

	if ( srcScreenIndex != destScreenIndex )
	{
		// keep TLHC in next screen same as current screen (relative to the working araea)
		CScreen curScreen = m_Screens[srcScreenIndex];
		CScreen otherScreen = m_Screens[destScreenIndex];
		RECT curScreenRect = curScreen.GetRect();
		RECT otherScreenRect = otherScreen.GetRect();
		int xOffset = otherScreenRect.left - curScreenRect.left;
		int yOffset = otherScreenRect.top - curScreenRect.top;
		destRect.left += xOffset;
		destRect.right += xOffset;
		destRect.top += yOffset;
		destRect.bottom += yOffset;
	}

	return destRect;
}

int MultiMonitors::FindHMonitor( HMONITOR hMonitor )
{
	int nScreenIndex = -1;
	for ( int nIndex = 0; nIndex < (int)m_Screens.size(); nIndex++ )
	{
		if ( m_Screens[nIndex].GetHMonitor() == hMonitor )
		{
			nScreenIndex = nIndex;
			break;
		}
	}

	return nScreenIndex;
}

// private static
void MultiMonitors::MoveWindow( HWND hWnd, WINDOWPLACEMENT* pWindowPlacement, const RECT& newRect )
{
	UINT oldShowCmd = pWindowPlacement->showCmd;

	if ( oldShowCmd == SW_SHOWMINIMIZED || oldShowCmd == SW_SHOWMAXIMIZED )
	{
		// first restore it
		pWindowPlacement->showCmd = SW_RESTORE;
		SetWindowPlacement( hWnd, pWindowPlacement );

		// now move it to the new screen
		pWindowPlacement->showCmd = SW_SHOW;
		pWindowPlacement->rcNormalPosition = newRect;
		SetWindowPlacement( hWnd, pWindowPlacement );

		// now min or maximise it
		pWindowPlacement->showCmd = oldShowCmd;
		SetWindowPlacement( hWnd, pWindowPlacement );
	} else
	{
		pWindowPlacement->rcNormalPosition = newRect;
		SetWindowPlacement( hWnd, pWindowPlacement );
	}
}

void MultiMonitors::AddMonitor( HMONITOR hMonitor, LPRECT lprcMonitor )
{
	//TRACE("MONITOR: 0x%0x %d,%d %d,%d\n", hMonitor,
	//	lprcMonitor->left, lprcMonitor->top, lprcMonitor->right, lprcMonitor->bottom);
	CScreen NewScreen( hMonitor, *lprcMonitor );
	m_Screens.push_back( NewScreen );
}


