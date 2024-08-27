// ATLXAML.cpp : Implementation of WinMain


#include "pch.h"
#include "ATLXAMLChild.h"
#include "ATLXAML.h"

LRESULT CATLXAMLFrame::OnAboutBox(HWND hWnd, WORD, WORD, HWND, BOOL&)
{
    CString sTitle;
    sTitle.LoadStringW(IDS_PROJNAME);
    ::ShellAbout(hWnd, sTitle, NULL, NULL);
    return 0;
}

LRESULT CATLXAMLFrame::OnFileNew(WORD, WORD, HWND, BOOL&)
{
    CATLXAMLChild* pMDIChild = new(std::nothrow)CATLXAMLChild;
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

LRESULT CATLXAMLFrame::OnInitMenu(UINT, WPARAM, LPARAM, BOOL& bHandled)
{
    //HWND hwndActive = HWND(::SendMessage(m_hMDIClient, WM_MDIGETACTIVE, 0, 0));
    //if(__nullptr != hwndActive)
    //{
        //std::vector<CATLXAMLChild*>::iterator itVec;
        //for(itVec = m_vecMDIChildren.begin(); itVec != m_vecMDIChildren.end(); ++itVec)
        //{
        //	CATLXAMLChild* thisChild = *itVec;
        //	if(hwndActive == thisChild->m_hWnd)
        //	{
        //	}
        //}
    //}
    return 0;
}


LRESULT CATLXAMLFrame::OnSize(UINT, WPARAM, LPARAM, BOOL&)
{
    return 0;
}

LRESULT CATLXAMLFrame::OnMDIDestroy(UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    bHandled = TRUE;
    std::vector<CATLXAMLChild*>::iterator itVec;
    for(itVec = m_vecMDIChildren.begin(); itVec != m_vecMDIChildren.end(); ++itVec)
    {
        CATLXAMLChild* thisChild = *itVec;
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

CATLXAMLModule _AtlXamlModule;

LRESULT CALLBACK CATLXAMLFrame::FrameWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    //CATLXAMLFrame* thisFrame = (CATLXAMLFrame*)::GetWindowLongPtr(hWnd, GWLP_USERDATA);
    //Normally the above is what one would do, but the ATL framework injects the
    //pointer to the CWindowImpl<CWinTraits> derived wrapper class, in the above HWND
    //This behavior is not documented but can be observed if stepping through atlwin.h
    CATLXAMLFrame* thisFrame = _AtlXamlModule.m_pFrame;// (CATLXAMLFrame*)hWnd;
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
            0, _T("MDICLIENT"), 0,
            WS_CLIPCHILDREN | WS_CHILD | WS_VISIBLE,
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

    return ::DefFrameProc(thisFrame->m_hWnd, thisFrame->m_hMDIClient, uMsg, wParam, lParam);
}

VOID CATLXAMLFrame::CommandDispatch(WORD wCommand, HWND hwnd, BOOL& bHandled)
{
    HWND hwndActive = HWND(::SendMessage(m_hMDIClient, WM_MDIGETACTIVE, 0, 0));
    if(__nullptr != hwndActive)
    {
        ::SendMessage(hwndActive, WM_COMMAND, wCommand, 0);
    }
}

HRESULT CATLXAMLModule::PreMessageLoop(int nCmdShow) throw()
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

    m_pFrame = new(std::nothrow) CATLXAMLFrame;
    if(NULL == m_pFrame)
    {
        __super::PostMessageLoop();
        return E_OUTOFMEMORY;
    }

    RECT rcPos = { CW_USEDEFAULT, 0, 0, 0 };//CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT};
    HMENU hMenu = ::LoadMenu(GetModuleHINSTANCE(), MAKEINTRESOURCE(IDR_MENU));
    HMENU hPaneMenu = ::LoadMenu(GetModuleHINSTANCE(), MAKEINTRESOURCE(IDR_PANE_MENU));
    HICON hIcon = ::LoadIcon(GetModuleHINSTANCE(), MAKEINTRESOURCE(IDI_ATLXAML));
    CString sTitle;
    (void)sTitle.LoadStringW(IDS_PROJNAME);
    m_pFrame->GetWndClassInfo().m_wc.hIcon = hIcon;
    HWND hwndFrame = m_pFrame->Create(0, rcPos, sTitle, 0, 0, hMenu, hPaneMenu);//, m_pFrame);
    if(__nullptr == hwndFrame)
    {
        return E_FAIL;
    }
    m_pFrame->m_hNoPaneMenu = hMenu;
    m_pFrame->ShowWindow(nCmdShow);
    m_pFrame->m_hSplitCursor = ::LoadCursor(__nullptr, MAKEINTRESOURCE(IDC_SIZEWE));
    m_hAccelTable = LoadAccelerators(GetModuleHINSTANCE(), MAKEINTRESOURCE(IDR_ACCELERATOR1));
    return hResult;
}

void CATLXAMLModule::RunMessageLoop() throw()
{
    MSG msg = {};
    while((GetMessage(&msg, 0, 0, 0) > 0))//WM_QUIT != msg.message
    {
        if(!::TranslateMDISysAccel(m_pFrame->m_hMDIClient, &msg))
        {
            if(0 == ::TranslateAccelerator(m_pFrame->m_hWnd, m_hAccelTable, &msg))
            {
                ::TranslateMessage(&msg);
                ::DispatchMessage(&msg);
            }
        }
    }
}

HRESULT CATLXAMLModule::PostMessageLoop() throw()
{
    HRESULT hr = __super::PostMessageLoop();
    if(NULL != m_pFrame)
    {
        delete m_pFrame;
        m_pFrame = NULL;
    }
    return hr;
}

//
extern "C" int WINAPI _tWinMain(HINSTANCE /*hInstance*/, HINSTANCE /*hPrevInstance*/,
    LPTSTR /*lpCmdLine*/, int nShowCmd)
{
    return _AtlXamlModule.WinMain(nShowCmd);
}

