// ATLSingleDocEdit.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "ATLSingleDocEdit.h"

LRESULT CATLSingleFrame::OnAboutBox(WORD , WORD , HWND , BOOL& )
{
	CString sTitle;
	BOOL bReturn = sTitle.LoadStringW(IDS_APP_TITLE);
    ::ShellAbout(m_hWnd, sTitle, NULL, NULL);
	return 0;
}

LRESULT CATLSingleFrame::OnSize(UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
   RECT rc;
   GetClientRect(&rc);
   ::SetWindowPos(m_hwndEdit, HWND_TOP, 0, 0, rc.right, rc.bottom, 0);
   //bHandled = FALSE;
   return 0;
}

LRESULT CATLEdit::OnKeyDown(UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	UINT keycode = static_cast<UINT>(wParam);// , LOWORD(lParam), HIWORD(lParam)
	//WPARAM numZoom = 0;
	//LPARAM denZoom = 0;
	//DWORD dwLastError = NO_ERROR;
	LRESULT bReturn = FALSE;
	bHandled = FALSE;
	switch(keycode)
	{
	case VK_ADD:
	case VK_OEM_PLUS:
		if((::GetKeyState(VK_CONTROL) >> 1))
		{
			//MessageBox(_T("Control and Plus pressed"));
			Edit_GetZoom(m_hWnd, &m_numZoom, &m_denZoom);
			if(0 == m_numZoom)
			{
				m_numZoom = 2;
			}
			else
			{
				m_numZoom *= 2;
			}
			if(0 == m_denZoom)
			{
				m_denZoom = 1;
			}
			Edit_SetZoom(m_hWnd, m_numZoom, m_denZoom);
			bHandled = TRUE;
			break;
		}
	case VK_OEM_MINUS:
	case VK_SUBTRACT:
		if((::GetKeyState(VK_CONTROL) >> 1))
		{
			Edit_GetZoom(m_hWnd, &m_numZoom, &m_denZoom);
			//MessageBox(_T("Control and Minus pressed"));
			if(0 == m_numZoom)
			{
				m_numZoom = 1;
			}
			if(0 == m_denZoom)
			{
				m_denZoom = 2;
			}
			else
			{
				m_denZoom *= 2;
			}
			Edit_SetZoom(m_hWnd, m_numZoom, m_denZoom);
			bHandled = TRUE;
			break;
		}
	case VK_NUMPAD0:
	case 0x30:
		if((::GetKeyState(VK_CONTROL) >> 1))
		{
			//MessageBox(_T("Control and Zero pressed"));
			m_numZoom = 0;
			m_denZoom = 0;
			Edit_SetZoom(m_hWnd, m_numZoom, m_denZoom);
			bHandled = TRUE;
			break;
		}
	}
	return 0;
}

LRESULT CATLSingleFrame::OnCreate(UINT , WPARAM , LPARAM , BOOL& )
{
	CATLEdit* pATLEdit = new CATLEdit;
	if(NULL != pATLEdit)
	{
		m_hwndEdit = pATLEdit->Create(m_hWnd, 0, __nullptr, 0, ES_EX_ZOOMABLE);
		if(0 == m_hwndEdit)
		{
			delete pATLEdit;
			MessageBox(_T("Edit creation failed!"));
		}
		else
		{
			RECT rc;
			GetClientRect(&rc);
			pATLEdit->SetWindowPos(HWND_TOP, &rc, SWP_SHOWWINDOW);
		}
	}

	return 0;
}

HRESULT CATLSingleDocModule::PreMessageLoop(int nCmdShow) throw()
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

	m_pFrame = new(std::nothrow) CATLSingleFrame;
	if(NULL == m_pFrame)
	{
		__super::PostMessageLoop();
		return E_OUTOFMEMORY;
	}

	RECT rcPos = {CW_USEDEFAULT, 0, 0, 0};//CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT};
	HMENU hMenu = ::LoadMenu(GetModuleHINSTANCE(), MAKEINTRESOURCE(IDC_ATLSINGLEDOCEDIT));
	HICON hIcon = ::LoadIcon(GetModuleHINSTANCE(), MAKEINTRESOURCE(IDI_ATLSINGLEDOCEDIT));
	CString sTitle(_T('\0'), 0x100);
	::LoadString(GetModuleHINSTANCE(), IDS_APP_TITLE, sTitle.GetBuffer(), 0x100);
	m_pFrame->GetWndClassInfo().m_wc.hIcon = hIcon;
	HWND hwndFrame = m_pFrame->Create(0, rcPos, sTitle, 0, 0, hMenu);//, m_pFrame);
	m_pFrame->ShowWindow(nCmdShow);

	m_hAccelTable = LoadAccelerators(GetModuleHINSTANCE(), MAKEINTRESOURCE(IDC_ATLSINGLEDOCEDIT));
	return hResult;
}

void CATLSingleDocModule::RunMessageLoop() throw()
{
	MSG msg = {};
	while((GetMessage(&msg, 0, 0, 0) > 0))//WM_QUIT != msg.message
	{
		if(0 == ::TranslateAccelerator(msg.hwnd, m_hAccelTable, &msg))
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}
	}
}

HRESULT CATLSingleDocModule::PostMessageLoop() throw()
{
	HRESULT hr = __super::PostMessageLoop();
	if(NULL != m_pFrame)
	{
		delete m_pFrame;
		m_pFrame = NULL;
	}
	return hr;
}


CATLSingleDocModule _AtlModule;



//
extern "C" int WINAPI _tWinMain(HINSTANCE /*hInstance*/, HINSTANCE /*hPrevInstance*/, 
								LPTSTR /*lpCmdLine*/, int nShowCmd)
{
	return _AtlModule.WinMain(nShowCmd);
}

//#define MAX_LOADSTRING 100
//
//// Global Variables:
//HINSTANCE hInst;								// current instance
//TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
//TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name
//
//// Forward declarations of functions included in this code module:
//ATOM				MyRegisterClass(HINSTANCE hInstance);
//BOOL				InitInstance(HINSTANCE, int);
//LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
//INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
//
//int APIENTRY _tWinMain(HINSTANCE hInstance,
//                     HINSTANCE hPrevInstance,
//                     LPTSTR    lpCmdLine,
//                     int       nCmdShow)
//{
//	UNREFERENCED_PARAMETER(hPrevInstance);
//	UNREFERENCED_PARAMETER(lpCmdLine);
//
// 	// TODO: Place code here.
//	MSG msg;
//	HACCEL hAccelTable;
//
//	// Initialize global strings
//	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
//	LoadString(hInstance, IDC_ATLSINGLEDOCEDIT, szWindowClass, MAX_LOADSTRING);
//	MyRegisterClass(hInstance);
//
//	// Perform application initialization:
//	if (!InitInstance (hInstance, nCmdShow))
//	{
//		return FALSE;
//	}
//
//	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_ATLSINGLEDOCEDIT));
//
//	// Main message loop:
//	while (GetMessage(&msg, NULL, 0, 0))
//	{
//		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
//		{
//			TranslateMessage(&msg);
//			DispatchMessage(&msg);
//		}
//	}
//
//	return (int) msg.wParam;
//}
//
//
//
////
////  FUNCTION: MyRegisterClass()
////
////  PURPOSE: Registers the window class.
////
////  COMMENTS:
////
////    This function and its usage are only necessary if you want this code
////    to be compatible with Win32 systems prior to the 'RegisterClassEx'
////    function that was added to Windows 95. It is important to call this function
////    so that the application will get 'well formed' small icons associated
////    with it.
////
//ATOM MyRegisterClass(HINSTANCE hInstance)
//{
//	WNDCLASSEX wcex;
//
//	wcex.cbSize = sizeof(WNDCLASSEX);
//
//	wcex.style			= CS_HREDRAW | CS_VREDRAW;
//	wcex.lpfnWndProc	= WndProc;
//	wcex.cbClsExtra		= 0;
//	wcex.cbWndExtra		= 0;
//	wcex.hInstance		= hInstance;
//	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ATLSINGLEDOCEDIT));
//	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
//	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
//	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_ATLSINGLEDOCEDIT);
//	wcex.lpszClassName	= szWindowClass;
//	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));
//
//	return RegisterClassEx(&wcex);
//}
//
////
////   FUNCTION: InitInstance(HINSTANCE, int)
////
////   PURPOSE: Saves instance handle and creates main window
////
////   COMMENTS:
////
////        In this function, we save the instance handle in a global variable and
////        create and display the main program window.
////
//BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
//{
//   HWND hWnd;
//
//   hInst = hInstance; // Store instance handle in our global variable
//
//   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
//      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);
//
//   if (!hWnd)
//   {
//      return FALSE;
//   }
//
//   ShowWindow(hWnd, nCmdShow);
//   UpdateWindow(hWnd);
//
//   return TRUE;
//}
//
////
////  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
////
////  PURPOSE:  Processes messages for the main window.
////
////  WM_COMMAND	- process the application menu
////  WM_PAINT	- Paint the main window
////  WM_DESTROY	- post a quit message and return
////
////
//LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
//{
//	int wmId, wmEvent;
//	PAINTSTRUCT ps;
//	HDC hdc;
//
//	switch (message)
//	{
//	case WM_COMMAND:
//		wmId    = LOWORD(wParam);
//		wmEvent = HIWORD(wParam);
//		// Parse the menu selections:
//		switch (wmId)
//		{
//		case IDM_ABOUT:
//			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
//			break;
//		case IDM_EXIT:
//			DestroyWindow(hWnd);
//			break;
//		default:
//			return DefWindowProc(hWnd, message, wParam, lParam);
//		}
//		break;
//	case WM_PAINT:
//		hdc = BeginPaint(hWnd, &ps);
//		// TODO: Add any drawing code here...
//		EndPaint(hWnd, &ps);
//		break;
//	case WM_DESTROY:
//		PostQuitMessage(0);
//		break;
//	default:
//		return DefWindowProc(hWnd, message, wParam, lParam);
//	}
//	return 0;
//}
//
//// Message handler for about box.
//INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
//{
//	UNREFERENCED_PARAMETER(lParam);
//	switch (message)
//	{
//	case WM_INITDIALOG:
//		return (INT_PTR)TRUE;
//
//	case WM_COMMAND:
//		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
//		{
//			EndDialog(hDlg, LOWORD(wParam));
//			return (INT_PTR)TRUE;
//		}
//		break;
//	}
//	return (INT_PTR)FALSE;
//}
