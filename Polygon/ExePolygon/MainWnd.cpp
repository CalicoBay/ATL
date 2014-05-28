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

LRESULT CMainWnd::OnCreate(UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
   CAtlEdit* pAtlEdit = new CAtlEdit;
   if(NULL != pAtlEdit)
   {
      m_hwndEdit = pAtlEdit->Create(m_hWnd);
      if(0 == m_hwndEdit)
      {
         delete pAtlEdit;
         MessageBox(_T("Edit creation failed!"));
      }
      else
      {
         RECT rc;
         GetClientRect(&rc);
         pAtlEdit->SetWindowPos(HWND_TOP, &rc, SWP_SHOWWINDOW);
      }
   }

   return 0;
}

LRESULT CMainWnd::OnSize(UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
   RECT rc;
   GetClientRect(&rc);
   ::SetWindowPos(m_hwndEdit, HWND_TOP, 0, 0, rc.right, rc.bottom, 0);
   //bHandled = FALSE;
   return 0;
}
