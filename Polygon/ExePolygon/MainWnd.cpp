// MainWnd.cpp : Implementation of CMainWnd

#include "stdafx.h"
#include "MainWnd.h"


// CMainWnd
STDMETHODIMP CMainWnd::CreateMain(HWND hWndParent, RECT rect, LPCTSTR szWindowName, DWORD dwStyle, DWORD dwExStyle, HMENU MenuOrID, LPARAM lpCreateParam)
{
	HRESULT hr = E_FAIL;
	HWND hwnd = Create(hWndParent, rect, szWindowName, dwStyle, dwExStyle, MenuOrID, (LPVOID)lpCreateParam);
	if (__nullptr != hwnd)
	{
		hr = S_OK;
		ShowWindow(SW_SHOW);
	}
	return hr;
}
