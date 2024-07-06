// ATL-MDI.cpp : Implementation of WinMain


#include "pch.h"
#include "ATL-MDIChild.h"
#include "ATL-MDI.h"
#include <App.xaml.h>
#include <MainPage.h>
#include <Microsoft.UI.Dispatching.Interop.h> // For ContentPreTranslateMessage

namespace winrt
{
	using namespace winrt::Microsoft::UI;
	using namespace winrt::Microsoft::UI::Dispatching;
	using namespace winrt::Microsoft::UI::Xaml;
	using namespace winrt::Microsoft::UI::Xaml::Hosting;
	using namespace winrt::Microsoft::UI::Xaml::Markup;
}

//bool ProcessMessageForTabNavigation(const HWND topLevelWindow, MSG* msg);

// Extra state for our top-level window, we point to from GWLP_USERDATA.
struct WindowInfo
{
	winrt::DesktopWindowXamlSource DesktopWindowXamlSource{ nullptr };
	winrt::event_token TakeFocusRequestedToken{};
	HWND LastFocusedWindow{ NULL };
};

// Returns "true" if the function handled the message and it shouldn't be processed any further.
// Intended to be called from the main message loop.
bool ProcessMessageForTabNavigation(const HWND topLevelWindow, MSG* msg)
{
	if(msg->message == WM_KEYDOWN && msg->wParam == VK_TAB)
	{
		// The user is pressing the "tab" key.  We want to handle this ourselves so we can pass information into Xaml
		// about the tab navigation.  Specifically, we need to tell Xaml whether this is a forward tab, or a backward
		// shift+tab, so Xaml will know whether to put focus on the first Xaml element in the island or the last
		// Xaml element.  (This is done in the call to DesktopWindowXamlSource.NavigateFocus()).
		const HWND currentFocusedWindow = ::GetFocus();
		if(::GetAncestor(currentFocusedWindow, GA_ROOT) != topLevelWindow)
		{
			// This is a window outside of our top-level window, let the system process it.
			return false;
		}

		const bool isShiftKeyDown = ((HIWORD(::GetKeyState(VK_SHIFT)) & 0x8000) != 0);
		const HWND nextFocusedWindow = ::GetNextDlgTabItem(topLevelWindow, currentFocusedWindow, isShiftKeyDown /*bPrevious*/);

		WindowInfo* windowInfo = reinterpret_cast<WindowInfo*>(::GetWindowLongPtr(topLevelWindow, GWLP_USERDATA));
		const HWND dwxsWindow = winrt::GetWindowFromWindowId(windowInfo->DesktopWindowXamlSource.SiteBridge().WindowId());
		if(dwxsWindow == nextFocusedWindow)
		{
			// Focus is moving to our DesktopWindowXamlSource.  Instead of just calling SetFocus on it, we call NavigateFocus(),
			// which allows us to tell Xaml which direction the keyboard focus is moving.
			// If your app has multiple DesktopWindowXamlSources in the window, you'll want to loop over them and check to
			// see if focus is moving to each one.
			winrt::XamlSourceFocusNavigationRequest request{
				 isShiftKeyDown ?
					  winrt::XamlSourceFocusNavigationReason::Last :
					  winrt::XamlSourceFocusNavigationReason::First };

			windowInfo->DesktopWindowXamlSource.NavigateFocus(request);
			return true;
		}

		// Focus isn't moving to our DesktopWindowXamlSource.  IsDialogMessage will automatically do the tab navigation
		// for us for this msg.
		const bool handled = (::IsDialogMessage(topLevelWindow, msg) == TRUE);
		return handled;
	}
	return false;
}

LRESULT CATLMDIFrame::OnAboutBox(HWND hWnd, WORD , WORD , HWND , BOOL& )
{
	CString sTitle;
	(void)sTitle.LoadStringW(IDS_PROJNAME);
    ::ShellAbout(hWnd, sTitle, NULL, NULL);
	return 0;
}

LRESULT CATLMDIFrame::OnFileNew(WORD, WORD, HWND, BOOL&)
{
	CATLMDIChild* pMDIChild = new(std::nothrow)CATLMDIChild;
	if(NULL != pMDIChild)
	{
		CString sTitle;
		std::vector<CString>::iterator itBegin = m_vecTitlesToRecycle.begin();
		std::vector<CString>::iterator itEnd = m_vecTitlesToRecycle.end();
		if(itBegin != itEnd)
		{
			std::sort(itBegin, itEnd, std::greater<CString>());
			sTitle = m_vecTitlesToRecycle.back();
			m_vecTitlesToRecycle.pop_back();
		}
		else
		{
			sTitle.Format(_T("%s%d"), m_sUntitled, m_vecMDIChildren.size());
		}

		pMDIChild->m_pfnSuperWindowProc = ::DefMDIChildProc;
		HWND hwndChild = pMDIChild->Create(m_hMDIClient, CWindow::rcDefault, sTitle, 0UL, 0UL, 0U, this);
		if(__nullptr != hwndChild)
		{
			m_vecMDIChildren.push_back(pMDIChild);
			if(1 == m_vecMDIChildren.size())
			{
				HMENU hCurrentMenu = GetMenu();
				if(hCurrentMenu != m_hPaneMenu)
				{
					HMENU hWindowMenu = ::GetSubMenu(m_hPaneMenu, WINDOW_MENU_POSITION);
					::SendMessage(m_hMDIClient, WM_MDISETMENU, WPARAM(m_hPaneMenu), LPARAM(hWindowMenu));
					DrawMenuBar();
				}
			}
         pMDIChild->m_Edit.SetFocus();
		}
		else
		{
			delete pMDIChild;
		}
	}
	return 0;
}

LRESULT CATLMDIFrame::OnInitMenu(UINT , WPARAM , LPARAM , BOOL& bHandled)
{
	//HWND hwndActive = HWND(::SendMessage(m_hMDIClient, WM_MDIGETACTIVE, 0, 0));
	//if(__nullptr != hwndActive)
	//{
		//std::vector<CATLMDIChild*>::iterator itVec;
		//for(itVec = m_vecMDIChildren.begin(); itVec != m_vecMDIChildren.end(); ++itVec)
		//{
		//	CATLMDIChild* thisChild = *itVec;
		//	if(hwndActive == thisChild->m_hWnd)
		//	{
		//	}
		//}
	//}
	return 0;
}


LRESULT CATLMDIFrame::OnSize(UINT , WPARAM , LPARAM , BOOL& )
{
	return 0;
}

LRESULT CATLMDIFrame::OnMDIDestroy(UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = TRUE;
	std::vector<CATLMDIChild*>::iterator itVec;
	for(itVec = m_vecMDIChildren.begin(); itVec != m_vecMDIChildren.end(); ++itVec)
	{
		CATLMDIChild* thisChild = *itVec;
		if(lParam == (LPARAM)thisChild)
		{
			CString sWinText((LPCTSTR)wParam);
			if(0 == sWinText.Find(m_sUntitled, 0))
			{
				m_vecTitlesToRecycle.push_back(sWinText);
			}

			m_vecMDIChildren.erase(itVec);
			break;
		}
	}

	if(0 == m_vecMDIChildren.size())
	{
		HMENU hCurrentMenu = GetMenu();
		if(hCurrentMenu != m_hNoPaneMenu)
		{
			HMENU hWindowMenu = __nullptr;
			::SendMessage(m_hMDIClient, WM_MDISETMENU, WPARAM(m_hNoPaneMenu), LPARAM(hWindowMenu));
			DrawMenuBar();
		}
	}

	return 0;
}


LRESULT CALLBACK CATLMDIFrame::FrameWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	//CATLMDIFrame* thisFrame = (CATLMDIFrame*)::GetWindowLongPtr(hWnd, GWLP_USERDATA);
	//Normally the above is what one would do, but the ATL framework injects the
	//pointer to the CWindowImpl<CWinTraits> derived wrapper class, in the above HWND
	//This behavior is not documented but can be observed if stepping through atlwin.h
	CATLMDIFrame* thisFrame = (CATLMDIFrame*)hWnd;
	BOOL bHandled = FALSE;
	switch(uMsg)
	{
	case WM_CREATE:
		{
			LPCREATESTRUCT pCreateStruct = (LPCREATESTRUCT)lParam;
			thisFrame->m_hPaneMenu = (HMENU)pCreateStruct->lpCreateParams;
			CLIENTCREATESTRUCT ccs = {};
			ccs.hWindowMenu = ::GetSubMenu(thisFrame->m_hPaneMenu, WINDOW_MENU_POSITION);
			ccs.idFirstChild = FIRST_MDI_CHILD;

			thisFrame->m_hMDIClient = ::CreateWindowEx(
								0, _T("MDICLIENT"),0, 
								WS_CLIPCHILDREN|WS_CHILD|WS_VISIBLE,
								0, 0, 0, 0, thisFrame->m_hWnd, HMENU(IDF_PANE),
								GetModuleHINSTANCE(), &ccs); 
			//LRESULT lResult = thisFrame->OnFrameCreate(hWnd, uMsg, wParam, lParam, bHandled);
			if(NULL == thisFrame->m_hMDIClient)
			{
				return -1;
			}
			
			//::SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)thisFrame);
			
			break;
		}
    case WM_NCDESTROY:
		{
			//::SetWindowLongPtr(hWnd, GWLP_USERDATA, NULL);
			LONG_PTR pWndProc = ::GetWindowLongPtr(thisFrame->m_hWnd, GWLP_WNDPROC);
			LRESULT lResult = ::DefFrameProc(thisFrame->m_hWnd, thisFrame->m_hMDIClient, uMsg, wParam, lParam);
			if(thisFrame->m_pfnSuperWindowProc != ::DefWindowProc && ::GetWindowLongPtr(thisFrame->m_hWnd, GWLP_WNDPROC) == pWndProc)
			{
				::SetWindowLongPtr(thisFrame->m_hWnd, GWLP_WNDPROC, (LONG_PTR)thisFrame->m_pfnSuperWindowProc);
			}

			HWND thisHWND = thisFrame->m_hWnd;
			thisFrame->m_hWnd = NULL;
			thisFrame->OnFinalMessage(thisHWND);

			return lResult;
		}

	case WM_INITMENU:
		{
			thisFrame->OnInitMenu(uMsg, wParam, lParam, bHandled);
			if(bHandled)
			{
				return 0;
			}
			break;
		}

	case WM_COMMAND:
		{
			switch(LOWORD(wParam))
			{
			case ID_FILE_NEW:
				{
					thisFrame->OnFileNew(HIWORD(wParam), LOWORD(wParam), (HWND)lParam, bHandled);
					break;
				}
			case ID_FILE_EXIT:
				{
					thisFrame->OnExit(thisFrame->m_hWnd, HIWORD(wParam), LOWORD(wParam), (HWND)lParam, bHandled);
					break;
				}
			case ID_CLEAR_TEXT:
				{
					thisFrame->CommandDispatch(ID_CLEAR_TEXT, HWND(lParam), bHandled);
					break;
				}
			case ID_SET_TEXT:
				{
					thisFrame->CommandDispatch(ID_SET_TEXT, HWND(lParam), bHandled);
					break;
				}
			case ID_WINDOW_CASCADE:
				{
					::SendMessage(thisFrame->m_hMDIClient, WM_MDICASCADE, 0, 0);
					break;
				}
			case ID_WINDOW_TILE_HORIZONTALLY:
				{
					::SendMessage(thisFrame->m_hMDIClient, WM_MDITILE, MDITILE_HORIZONTAL, 0);
					break;
				}
			case ID_WINDOW_TILE_VERTICALLY:
				{
					::SendMessage(thisFrame->m_hMDIClient, WM_MDITILE, MDITILE_VERTICAL, 0);
					break;
				}
			case ID_WINDOW_ARRANGE_ICONS:
				{
					::SendMessage(thisFrame->m_hMDIClient, WM_MDIICONARRANGE, 0, 0);
					break;
				}
			case ID_HELP_ABOUT:
				{
					thisFrame->OnAboutBox(thisFrame->m_hWnd, HIWORD(wParam), LOWORD(wParam), (HWND)lParam, bHandled);
					break;
				}
			}

			break;
		}
	}

	return ::DefFrameProc(thisFrame->m_hWnd, (NULL == thisFrame ? NULL : thisFrame->m_hMDIClient), uMsg, wParam, lParam);
}

VOID CATLMDIFrame::CommandDispatch(WORD wCommand, HWND hwnd, BOOL& bHandled)
{
	HWND hwndActive = HWND(::SendMessage(m_hMDIClient, WM_MDIGETACTIVE, 0, 0));
	if(__nullptr != hwndActive)
	{
		::SendMessage(hwndActive, WM_COMMAND, wCommand, 0);
	}
}

HRESULT CATLMDIModule::PreMessageLoop(int nCmdShow) throw()
{
	HRESULT hResult = S_OK;
	//hResult = __super::PreMessageLoop(nCmdShow);
	//if(FAILED(hResult))
	//{
	//	return hResult;
	//}
	//else
	//{//RegisterClassObjects returns S_FALSE if there were no classes to register
	//	//but if PreMessageLoop doesn't return S_OK RunMessageLoop won't run
	//	hResult = S_OK;
	//}
	//m_pDispatcherQueueController = &(winrt::Microsoft::UI::Dispatching::DispatcherQueueController::CreateOnCurrentThread());
	//winrt::uninit_apartment();
	//m_pDispatcherQueueController = new winrt::Microsoft::UI::Dispatching::DispatcherQueueController;

	m_pFrame = new(std::nothrow) CATLMDIFrame;
	if(NULL == m_pFrame)
	{
		__super::PostMessageLoop();
		return E_OUTOFMEMORY;
	}

	RECT rcPos = {CW_USEDEFAULT, 0, 0, 0};//CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT};
	HMENU hMenu = ::LoadMenu(GetModuleHINSTANCE(), MAKEINTRESOURCE(IDR_MENU));
	HMENU hPaneMenu = ::LoadMenu(GetModuleHINSTANCE(), MAKEINTRESOURCE(IDR_PANE_MENU));
	HICON hIcon = ::LoadIcon(GetModuleHINSTANCE(), MAKEINTRESOURCE(IDI_ATLMDI));
	CString sTitle;
	sTitle.LoadStringW(IDS_PROJNAME);
	m_pFrame->GetWndClassInfo().m_wc.hIcon = hIcon;
	HWND hwndFrame = m_pFrame->Create(0, rcPos, sTitle, 0, 0, hMenu, hPaneMenu);//, m_pFrame);
	if(__nullptr == hwndFrame)
	{
		return E_FAIL;
	}
	m_pFrame->m_hNoPaneMenu = hMenu;
	m_pFrame->ShowWindow(nCmdShow);
	m_pFrame->m_hSplitCursor = ::LoadCursor(__nullptr, MAKEINTRESOURCE(IDC_SIZENS));
   m_hAccelTable = LoadAccelerators(GetModuleHINSTANCE(), MAKEINTRESOURCE(IDR_ACCELERATOR1));
	return hResult;
}

void CATLMDIModule::RunMessageLoop() throw()
{
	try
	{
		auto dispatcherQueueController{ winrt::DispatcherQueueController::CreateOnCurrentThread() };
		// Island-support: Create our custom Xaml App object. This is needed to properly use the controls and metadata
		// in Microsoft.ui.xaml.controls.dll.
		auto simpleIslandApp{ winrt::make<winrt::ATLMDI::implementation::App>() };

		MSG msg = {};

		while((GetMessage(&msg, 0, 0, 0) > 0))//WM_QUIT != msg.message
		{
			if(!::TranslateMDISysAccel(m_pFrame->m_hMDIClient, &msg))
			{
				if(::ContentPreTranslateMessage(&msg))
				{
					continue;
				}
				if(::TranslateAccelerator(m_pFrame->m_hWnd, m_hAccelTable, &msg))
				{
					continue;
				}

				// Island-support: This is needed so that the user can correctly tab and shift+tab into islands.
				if(ProcessMessageForTabNavigation(m_pFrame->m_hWnd, &msg))
				{
					continue;
				}

				::TranslateMessage(&msg);
				::DispatchMessage(&msg);
			}
		}

		// Island-support: To properly shut down after using a DispatcherQueue, call ShutdownQueue[Aysnc]().
		dispatcherQueueController.ShutdownQueue();
	}
	catch(const winrt::hresult_error& exception)
	{
		int32_t iResult = exception.code().value;
	}
}

HRESULT CATLMDIModule::PostMessageLoop() throw()
{
	HRESULT hr = __super::PostMessageLoop();
	if(NULL != m_pFrame)
	{
		delete m_pFrame;
		m_pFrame = NULL;
	}
	return hr;
}


CATLMDIModule _AtlModule;



//
extern "C" int WINAPI _tWinMain(HINSTANCE /*hInstance*/, HINSTANCE /*hPrevInstance*/, 
								LPTSTR /*lpCmdLine*/, int nShowCmd)
{
	//winrt::uninit_apartment();
	winrt::init_apartment(winrt::apartment_type::single_threaded);//winrt::apartment_type::single_threaded

	int iReturn = _AtlModule.WinMain(nShowCmd);

	return iReturn;
}

