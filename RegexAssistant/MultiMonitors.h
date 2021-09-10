#pragma once

#include <wtypes.h>
#include <tchar.h>
#include <vector>
#include "Screen.h"

/*********************************************************************************************
Example Usage:
----------------------------------------------------------------------------------------------
#include "MultiMonitors.h"
if ( m_MonitorToDisplay > 0 )
{
	MultiMonitors multimonitors;
	multimonitors.MoveWindowToMonitor( this->m_hWnd, static_cast<UINT>(m_MonitorToDisplay) );
}
*********************************************************************************************/


class MultiMonitorsData;

class MultiMonitors
{
protected:
	MultiMonitorsData*			m_pData;
	int							m_nPrimaryMonitorIndex;
	RECT						m_rcVirtualScrn;
	std::vector<CScreen>        m_Screens;
	BOOL                        m_EnumDisplayMonitorsSuccess;
public:
	MultiMonitors();
	~MultiMonitors();
	BOOL MoveWindowToMonitor( HWND hwnd, UINT nMonitor_Index );
	UINT GetMonitorCount();
	SIZE GetMonitorResolution( UINT nMonitor_Index );
	RECT GetMonitorPosition( UINT nMonitor_Index );
	UINT GetPrimaryMonitorIndex();
	inline RECT GetVirtualScreen() { return m_rcVirtualScrn; }
	inline BOOL Get_EnumDisplayMonitorsSuccess() { return m_EnumDisplayMonitorsSuccess; }
protected:
	static BOOL CALLBACK _MonitorEnumProc( HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData );
	BOOL MonitorEnumProc( HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor );
	RECT TransformRectToOtherScreen( const RECT& srcRect, int otherScreenIndex );
	RECT TransformRect( const RECT& srcRect, int srcScreenIndex, int destScreenIndex );
private:
	int FindHMonitor( HMONITOR hMonitor );
	static void MoveWindow( HWND hWnd, WINDOWPLACEMENT* pWindowPlacement, const RECT& newRect );
	void AddMonitor( HMONITOR hMonitor, LPRECT lprcMonitor );
};
