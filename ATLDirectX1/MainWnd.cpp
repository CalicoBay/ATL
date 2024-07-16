#include "stdafx.h"
#include "D2DTab.h"
#include "D3DTab.h"
#include "EditableLayout.h"
#include "DrawingEffects.h"
#include "RenderTarget.h"
#include "InlineImage.h"
#include "DWriteTab.h"
#include "D3DCubeTab.h"
#include "MainWnd.h"

LONG CDxMain::m_cTabCount = 0;
LONG CDxMain::m_vecTabCapacity = 0x10;

CDxMain::CDxMain() : m_hWndCurrentTab(0)
{
	m_vecTabWindows.resize(m_vecTabCapacity, __nullptr);
}
CDxMain::~CDxMain()
{
}

HRESULT CDxMain::Initialize()
{
	HRESULT hResult;
	HMENU hMenu = LoadMenu(GetModuleHINSTANCE(), MAKEINTRESOURCE(IDC_ATLDX1));
	HICON hIcon = LoadIcon(_AtlBaseModule.GetResourceInstance(), MAKEINTRESOURCE(IDI_ICON1));
	GetWndClassInfo().m_wc.hIcon = hIcon;
	HWND hwndThis = Create(0, CWindow::rcDefault, TEXT("ATLDirectX One"), 0, 0, hMenu);
	hResult = (0 != hwndThis) ? S_OK : E_FAIL;
	if(SUCCEEDED(hResult))
	{
		RECT rcClient;
		GetClientRect(&rcClient);
		HWND hwndTabControl = ::CreateWindowEx(0, WC_TABCONTROL, _T(""), WS_CHILD|WS_CLIPSIBLINGS|WS_VISIBLE, 0, 0,
												rcClient.right, rcClient.bottom, m_hWnd, 0U, GetModuleHINSTANCE(), __nullptr); 
		hResult = (0 != hwndTabControl) ? S_OK : E_FAIL;
		if(SUCCEEDED(hResult))
		{
			m_TabControl.Attach(hwndTabControl);
			TCITEM tcItem;
			tcItem.mask = TCIF_IMAGE|TCIF_TEXT;
			tcItem.iImage = -1;
			tcItem.pszText = _T("DirectWrite");
			int iReturn = TabCtrl_InsertItem(hwndTabControl, m_cTabCount, &tcItem);
			if(-1 != iReturn)
			{
            CDWriteTab* pDWriteTab = new(std::nothrow)CDWriteTab;
				if(__nullptr != pDWriteTab)
				{
					hResult = pDWriteTab->Initialize(m_hWnd, _T("Some Text"));
					if(SUCCEEDED(hResult))
					{
						m_hWndCurrentTab = pDWriteTab->operator HWND();
						m_vecTabWindows[m_cTabCount] = pDWriteTab;
						::InterlockedIncrement(&m_cTabCount);
					}
					else
					{
						pDWriteTab->DestroyWindow();
						TabCtrl_DeleteItem(m_TabControl, m_cTabCount);
						iReturn = -1;
					}
				}
			}

			if(-1 != iReturn)
			{
				tcItem.pszText = _T("Direct2D");
				iReturn = TabCtrl_InsertItem(hwndTabControl, m_cTabCount, &tcItem);
			}

			if(-1 != iReturn)
			{
				CD2DTab* pD2DTab = new(std::nothrow)CD2DTab;
				if(__nullptr != pD2DTab)
				{
					hResult = pD2DTab->Initialize(m_hWnd);
					if(SUCCEEDED(hResult))
					{
						m_vecTabWindows[m_cTabCount] = pD2DTab;
						::InterlockedIncrement(&m_cTabCount);
					}
					else
					{
						pD2DTab->DestroyWindow();
						TabCtrl_DeleteItem(m_TabControl, m_cTabCount);
						iReturn = -1;
					}
				}
			}
	
			if(-1 != iReturn)
			{
				tcItem.pszText = _T("Direct3D");
				iReturn = TabCtrl_InsertItem(hwndTabControl, m_cTabCount, &tcItem);
			}

			if(-1 != iReturn)
			{
				CD3DTab* pD3DTab = new(std::nothrow)CD3DTab;
				if(__nullptr != pD3DTab)
				{
					hResult = pD3DTab->Initialize(m_hWnd);
					if(SUCCEEDED(hResult))
					{
						m_vecTabWindows[m_cTabCount] = pD3DTab;
						::InterlockedIncrement(&m_cTabCount);
					}
					else
					{
						pD3DTab->DestroyWindow();
						TabCtrl_DeleteItem(m_TabControl, m_cTabCount);
						iReturn = -1;
					}
				}
			}
	
			if(-1 != iReturn)
			{
				tcItem.pszText = _T("Direct3D Cube");
				iReturn = TabCtrl_InsertItem(hwndTabControl, m_cTabCount, &tcItem);
			}

			if(-1 != iReturn)
			{
				CD3DCubeTab* pD3DTab = new(std::nothrow)CD3DCubeTab;
				if(__nullptr != pD3DTab)
				{
					hResult = pD3DTab->Initialize(m_hWnd);
					if(SUCCEEDED(hResult))
					{
						m_vecTabWindows[m_cTabCount] = pD3DTab;
						::InterlockedIncrement(&m_cTabCount);
					}
					else
					{
						//TODO: Display hResult
						pD3DTab->DestroyWindow();
						TabCtrl_DeleteItem(m_TabControl, m_cTabCount);
						iReturn = -1;
					}
				}
			}
	
			if(-1 == iReturn)
			{
				BOOL bHandled = TRUE;
				OnExit(0, 0, 0, bHandled);
				return S_FALSE;
			}

			ShowWindow(SW_SHOWNORMAL);
			UpdateWindow();
		}
	}
	return hResult;
}

LRESULT CDxMain::OnCreate(UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return 0;
}

LRESULT CDxMain::OnGetMinMaxInfo(UINT /*nMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
{
	PMINMAXINFO pMinMaxInfo = reinterpret_cast<PMINMAXINFO>(lParam);
    pMinMaxInfo->ptMinTrackSize.x = 200;
    pMinMaxInfo->ptMinTrackSize.y = 200;
	return 0;
}

LRESULT CDxMain::OnPaint(UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    PAINTSTRUCT ps;
	HDC  hdc = BeginPaint(&ps);
	EndPaint(&ps);
    return 0;
}

LRESULT CDxMain::OnSize(UINT /*nMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
{
	LONG width = LOWORD(lParam);
	LONG height = HIWORD(lParam);
	RECT clientRect = {0, 0, width, height};

	if(0 == m_hWndCurrentTab)
	{
		m_TabControl.SetWindowPos(__nullptr, &clientRect, SWP_NOACTIVATE|SWP_NOZORDER);
		return 0;
	}

	TabCtrl_AdjustRect(m_TabControl, FALSE, &clientRect);

	HDWP hdwp = ::BeginDeferWindowPos(2);
	::DeferWindowPos(hdwp, m_TabControl, __nullptr, 0, 0, width, height, SWP_NOMOVE|SWP_NOZORDER);
	::DeferWindowPos(hdwp, m_hWndCurrentTab, HWND_TOP, clientRect.left, clientRect.top, clientRect.right - clientRect.left, clientRect.bottom - clientRect.top, 0);
	::EndDeferWindowPos(hdwp);
	return 0;
}

LRESULT CDxMain::OnTabSelChange(int idCtrl, LPNMHDR pnmh, BOOL& bHandled)
{
	int iSel = TabCtrl_GetCurSel(m_TabControl);
	if(iSel > m_vecTabWindows.size())
	{
		return 0;
	}

	::ShowWindow(m_hWndCurrentTab, SW_HIDE);
	CWindow* pSelectedTab = m_vecTabWindows[iSel];
	if(__nullptr != pSelectedTab)
	{
		m_hWndCurrentTab = pSelectedTab->operator HWND();
	}

	::ShowWindow(m_hWndCurrentTab, SW_SHOWNORMAL);
	//Call OnSize so the first time a new child is displayed
	//it doesn't overlap the tab control.
	RECT rc;
	GetClientRect(&rc);
	LPARAM lParam = MAKELPARAM((rc.right - rc.left), (rc.bottom - rc.top));
	OnSize(0, 0, lParam, bHandled);

	return 0;
}

VOID CDxMain::PaintCurrentTab()
{
	::SendMessage(m_hWndCurrentTab, WM_PAINT, WPARAM(m_hWnd), LPARAM(0));
}

INT_PTR CALLBACK CDxMain::About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}