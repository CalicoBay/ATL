// MultiRich.cpp : Defines the entry point for the console application.
//
// ATLMultiDocEdit.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "BomFile.h"
#include "ATLRichEdit.h"
#include "MultiRichChild.h"
#include "MultiRich.h"

FINDREPLACE CMultiRichFrame::s_FIND = {};
FINDREPLACE CMultiRichFrame::s_FINDREPLACE = {};
HWND CMultiRichFrame::s_hwndCurrentDlg = __nullptr;
HWND CMultiRichFrame::s_hwndFindDlg = __nullptr;
HWND CMultiRichFrame::s_hwndFindReplaceDlg = __nullptr;
HWND CMultiRichFrame::s_hwndThisFrame = __nullptr;
TCHAR CMultiRichFrame::s_tszFindWhat[0x80] = {};
TCHAR CMultiRichFrame::s_tszReplaceWhat[0x80] = {};
TCHAR CMultiRichFrame::s_tszReplaceWith[0x80] = {};
UINT CMultiRichFrame::s_FindReplaceMsg = 0U;

CMultiRichFrame::CMultiRichFrame():
	m_NewCount(0U),
	m_hMDIClient(__nullptr),
	m_hMainFrameMenu(__nullptr),
	m_hPregnantMenu(__nullptr),
	m_hChildIcon(__nullptr)
{
	m_bStringLoaded = m_sUntitled.LoadStringW(IDS_UNTITLED);
	s_FindReplaceMsg = ::RegisterWindowMessage(FINDMSGSTRING);
}

VOID CMultiRichFrame::InitFIND()
{
	::ZeroMemory(&s_FIND, sizeof(s_FIND));
	s_FIND.Flags |= (FR_ENABLEHOOK | FR_DOWN);
	s_FIND.hInstance = GetModuleHINSTANCE();
	s_FIND.hwndOwner = s_hwndThisFrame;
	//s_FIND.lCustData = L'F';
	s_FIND.lpfnHook = CMultiRichFrame::FindHookProc;
	s_FIND.lpstrFindWhat = s_tszFindWhat;
	s_FIND.lStructSize = sizeof(s_FINDREPLACE);
	s_FIND.wFindWhatLen = 0x80;
}

VOID CMultiRichFrame::InitFINDREPLACE()
{
	::ZeroMemory(&s_FINDREPLACE, sizeof(s_FINDREPLACE));
	s_FINDREPLACE.Flags |= (FR_ENABLEHOOK | FR_DOWN);
	s_FINDREPLACE.hInstance = GetModuleHINSTANCE();
	s_FINDREPLACE.hwndOwner = s_hwndThisFrame;
	//s_FINDREPLACE.lCustData = L'R';
	s_FINDREPLACE.lpfnHook = CMultiRichFrame::FindReplaceHookProc;
	s_FINDREPLACE.lpstrFindWhat = s_tszReplaceWhat;
	s_FINDREPLACE.lpstrReplaceWith = s_tszReplaceWith;
	s_FINDREPLACE.lStructSize = sizeof(s_FINDREPLACE);
	s_FINDREPLACE.wFindWhatLen = 0x80;
	s_FINDREPLACE.wReplaceWithLen = 0x80;
}

LRESULT CMultiRichFrame::OnAboutBox(HWND hWnd, WORD , WORD , HWND , BOOL& )
{
	CString sTitle;
	m_bStringLoaded = sTitle.LoadStringW(IDS_APP_TITLE);
    ::ShellAbout(hWnd, sTitle, NULL, NULL);
	return 0;
}

LRESULT CMultiRichFrame::OnFileNew(WORD, WORD, HWND, BOOL&)
{
	CMultiRichChild* pMDIChild = new(std::nothrow)CMultiRichChild;
	if(__nullptr != pMDIChild)
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
			sTitle.Format(_T("%s%d"), (LPCTSTR)m_sUntitled, m_NewCount++);
		}

		pMDIChild->GetWndClassInfo().m_wc.hIcon = m_hChildIcon;
		pMDIChild->m_pfnSuperWindowProc = ::DefMDIChildProc;
		HWND hwndChild = pMDIChild->Create(m_hMDIClient, 0, sTitle, 0UL, 0UL, 0U, this);
		if(__nullptr != hwndChild)
		{
			m_vecMDIChildren.push_back(pMDIChild);
			if(1 == m_vecMDIChildren.size())
			{//If needed show the right menus
				HMENU hCurrentMenu = GetMenu();
				if(hCurrentMenu != m_hPregnantMenu)
				{
					HMENU hWindowMenu = ::GetSubMenu(m_hPregnantMenu, WINDOW_MENU_POSITION);
					::SendMessage(m_hMDIClient, WM_MDISETMENU, WPARAM(m_hPregnantMenu), LPARAM(hWindowMenu));
					DrawMenuBar();
				}
			}
		}
		else
		{
			delete pMDIChild;
		}
	}
	return 0;
}

LRESULT CMultiRichFrame::OnFileOpen(WORD, WORD, HWND, BOOL&)
{
	LRESULT lr = 0;
	HRESULT hr;
	Microsoft::WRL::ComPtr<IFileDialog> cpFileDialog;
	Microsoft::WRL::ComPtr<IShellItem> cpShellItem;
	PWSTR wszFilePath = __nullptr;
	hr = ::CoCreateInstance(CLSID_FileOpenDialog, __nullptr, CLSCTX_INPROC_SERVER, __uuidof(IFileDialog), &cpFileDialog);
	if(SUCCEEDED(hr))
	{
		FILEOPENDIALOGOPTIONS fos;
		hr = cpFileDialog->GetOptions(&fos);
		if(SUCCEEDED(hr))
		{
			cpFileDialog->SetOptions(fos|FOS_FORCEFILESYSTEM);
		}
		hr = cpFileDialog->Show(__nullptr);
	}

	::SetForegroundWindow(m_hWnd);

	if(SUCCEEDED(hr))
	{
		
		hr = cpFileDialog->GetResult(&cpShellItem);
		if(SUCCEEDED(hr))
		{
			hr = cpShellItem->GetDisplayName(SIGDN_FILESYSPATH, &wszFilePath);
		}
		if(SUCCEEDED(hr))
		{
			CMultiRichChild* pMDIChild = new(std::nothrow)CMultiRichChild;
			if(NULL != pMDIChild)
			{
				pMDIChild->GetWndClassInfo().m_wc.hIcon = m_hChildIcon;
				pMDIChild->m_pfnSuperWindowProc = ::DefMDIChildProc;
				HWND hwndChild = pMDIChild->Create(m_hMDIClient, 0, wszFilePath, 0UL, 0UL, 0U, this);
				if(NULL != hwndChild)
				{
					m_vecMDIChildren.push_back(pMDIChild);
					m_vecMRUs.push_back(wszFilePath);
					if(1 == m_vecMDIChildren.size())
					{//If needed show the right menus
						HMENU hCurrentMenu = GetMenu();
						if(hCurrentMenu != m_hPregnantMenu)
						{
							HMENU hWindowMenu = ::GetSubMenu(m_hPregnantMenu, WINDOW_MENU_POSITION);
							::SendMessage(m_hMDIClient, WM_MDISETMENU, WPARAM(m_hPregnantMenu), LPARAM(hWindowMenu));
							DrawMenuBar();
							//::SendMessage(m_hMDIClient, WM_MDITILE, MDITILE_HORIZONTAL, 0);//This works
							//::ShowWindow(hwndChild, SW_MAXIMIZE);//This also works
							::SetWindowPos(hwndChild, HWND_TOP, 1, 1, 0, 0, SWP_NOSIZE);//This works best
							//The first MDIChild was not displaying text when opened from a file
						}
					}
				}
				else
				{
					delete pMDIChild;
				}
			}
			::CoTaskMemFree(wszFilePath);
		}
	}
	return lr;
}

LRESULT CMultiRichFrame::OnFileSave(WORD, WORD, HWND, BOOL& bHandled)
{
	HRESULT hr = E_FAIL;
	HWND hwndActive = HWND(::SendMessage(m_hMDIClient, WM_MDIGETACTIVE, 0, 0));
	if(__nullptr != hwndActive)
	{
		CATLRichEdit* pATLEdit = __nullptr;
		std::vector<CMultiRichChild*>::iterator itVec;
		for(itVec = m_vecMDIChildren.begin(); itVec != m_vecMDIChildren.end(); ++itVec)
		{
			CMultiRichChild* thisChild = *itVec;
			if(hwndActive == thisChild->m_hWnd)
			{
				pATLEdit = thisChild->m_pATLRichEdit;
				hr = pATLEdit->SaveFile();//wszFilePath, GENERIC_READ, FILE_SHARE_READ|FILE_SHARE_WRITE, OPEN_EXISTING);
				break;
			}
		}
	}

	if(S_OK != hr)
	{
		if(0x80070003 == hr)
		{//HRESULT_FROM_WIN32(ERROR_PATH_NOT_FOUND)
			OnFileSaveAs(0, 0, 0, bHandled);
		}
		else
		{
			int iResult;
			if(SUCCEEDED(::TaskDialog(__nullptr, __nullptr, __nullptr, __nullptr,
				L"The file could not be saved! Do you wish to save your changes using another path?",
					TDCBF_YES_BUTTON|TDCBF_CANCEL_BUTTON, TD_WARNING_ICON, &iResult)))
			{
				if(IDYES == iResult)
				{
					OnFileSaveAs(0, 0, 0, bHandled);
				}
			}
		}
	}
	return 0;
}

LRESULT CMultiRichFrame::OnFileSaveAs(WORD, WORD, HWND, BOOL&)
{
	HRESULT hr;
	Microsoft::WRL::ComPtr<IFileSaveDialog> cpFileDialog;
	Microsoft::WRL::ComPtr<IShellItem> cpShellItem;
	PWSTR wszFilePath = __nullptr;
	CATLRichEdit* activeEdit = __nullptr;
	CMultiRichChild* activeChild = __nullptr;
	HWND hwndActive = HWND(::SendMessage(m_hMDIClient, WM_MDIGETACTIVE, 0, 0));
	if(__nullptr != hwndActive)
	{
		std::vector<CMultiRichChild*>::iterator itVec;
		for(itVec = m_vecMDIChildren.begin(); itVec != m_vecMDIChildren.end(); ++itVec)
		{
			CMultiRichChild* thisChild = *itVec;
			if(hwndActive == thisChild->m_hWnd)
			{
				activeEdit = thisChild->m_pATLRichEdit;
				activeChild = thisChild;
				break;
			}
		}
	}

	if(__nullptr != activeChild && __nullptr != activeEdit)
	{
		hr = ::CoCreateInstance(CLSID_FileSaveDialog, __nullptr, CLSCTX_INPROC_SERVER, __uuidof(IFileSaveDialog), &cpFileDialog);
		if(SUCCEEDED(hr))
		{
			UINT fileTypeIndex = 0;
			LPCWSTR pName = activeChild->m_sOriginalTitle;
			LPCWSTR pExtHint = activeEdit->GetExtensionHint(fileTypeIndex);
			hr = cpFileDialog->SetFileTypes(ARRAYSIZE(c_RichEditFileTypes), c_RichEditFileTypes);
			if(SUCCEEDED(hr))hr = cpFileDialog->SetFileTypeIndex(fileTypeIndex);
			if(SUCCEEDED(hr))hr = cpFileDialog->SetDefaultExtension(pExtHint);
			if(SUCCEEDED(hr))cpFileDialog->SetFileName(pName);
			if(SUCCEEDED(hr))cpFileDialog->SetFileNameLabel(L"File Name");
			if(SUCCEEDED(hr))hr = cpFileDialog->Show(__nullptr);
		}
		if(SUCCEEDED(hr))
		{
			hr = cpFileDialog->GetResult(&cpShellItem);
			if(SUCCEEDED(hr))
			{
				hr = cpShellItem->GetDisplayName(SIGDN_FILESYSPATH, &wszFilePath);
				if(SUCCEEDED(hr))
				{
					if(0 == activeChild->m_sOriginalTitle.Find(m_sUntitled, 0))
					{
						m_vecTitlesToRecycle.push_back(activeChild->m_sOriginalTitle);
					}
					hr = activeEdit->SaveFile(wszFilePath);
					if(SUCCEEDED(hr))
					{
						activeChild->SetWindowTextW(wszFilePath);
					}

					if(S_OK != hr)
					{
						CStringW sMessage;
						sMessage.Format(L"The file\r\n%s\r\ncould not be saved!\r\nHRESULT: 0x%08X", wszFilePath, hr);
						::TaskDialog(__nullptr, __nullptr, __nullptr, __nullptr,
							sMessage, TDCBF_YES_BUTTON, TD_WARNING_ICON, __nullptr);
					}

					::CoTaskMemFree(wszFilePath);
				}
			}
		}
	}
	return 0;
}

LRESULT CMultiRichFrame::OnOpenRecentFile(WORD, WORD, HWND, BOOL&)
{
	LRESULT lr = 0;
	MessageBox(_T("in OnOpenRecentFile"), _T("MultiRich"));
	return lr;
}
LRESULT CMultiRichFrame::OnInitMenu(UINT , WPARAM , LPARAM , BOOL& )
{
	return 0;
}

LRESULT CMultiRichFrame::OnSize(UINT , WPARAM , LPARAM , BOOL& )
{
	return 0;
}

LRESULT CMultiRichFrame::OnMDIDestroy(UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = TRUE;
	std::vector<CMultiRichChild*>::iterator itVec;
	for(itVec = m_vecMDIChildren.begin(); itVec != m_vecMDIChildren.end(); ++itVec)
	{
		CMultiRichChild* thisChild = *itVec;
		if(lParam == (LPARAM)thisChild)
		{
			CString sWinText((LPCTSTR)wParam);
			//if(thisChild->m_pATLRichEdit->IsDirty())// Too late for this at this time the window is already gone. IsDirty throws.
			//{
			//	int iResult;
			//	if(SUCCEEDED(::TaskDialog(__nullptr, __nullptr, __nullptr, sWinText, L"Do you wish to save your changes?", TDCBF_YES_BUTTON | TDCBF_NO_BUTTON | TDCBF_CANCEL_BUTTON, TD_WARNING_ICON, &iResult)))
			//	{
			//		if(IDYES == iResult)
			//		{
			//			MessageBox(_T("Sending you back to save."), sWinText);
			//			return S_FALSE;
			//		}
			//	}
			//}
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
		if(hCurrentMenu != m_hMainFrameMenu)
		{
			HMENU hWindowMenu = __nullptr;
			::SendMessage(m_hMDIClient, WM_MDISETMENU, WPARAM(m_hMainFrameMenu), LPARAM(hWindowMenu));
			DrawMenuBar();
		}
	}

	return 0;
}


LRESULT CALLBACK CMultiRichFrame::FrameWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	//CMultiRichFrame* thisFrame = (CMultiRichFrame*)::GetWindowLongPtr(hWnd, GWLP_USERDATA);
	//Normally the above is what one would do, but the ATL framework injects the
	//pointer to the CWindowImpl<CWinTraits> derived wrapper class, in the above HWND
	//This behavior is not documented but can be observed if stepping through atlwin.h
	CMultiRichFrame* thisFrame = (CMultiRichFrame*)hWnd;
	BOOL bHandled = FALSE;
	switch(uMsg)
	{
	case WM_CREATE:
		{
			CLIENTCREATESTRUCT ccs = {};
			ccs.hWindowMenu = ::GetSubMenu(::GetMenu(thisFrame->m_hWnd), WINDOW_MENU_POSITION);
			//TODO: Check out above if we need it
			ccs.idFirstChild = FIRST_MDI_CHILD;

			thisFrame->m_hMDIClient = ::CreateWindowEx(
								0, _T("MDICLIENT"),0, 
								WS_CLIPCHILDREN|WS_CHILD|WS_VISIBLE|WS_VSCROLL|WS_HSCROLL,
								0, 0, 0, 0, thisFrame->m_hWnd, HMENU(ID_FIRST_PANE),
								GetModuleHINSTANCE(), &ccs); 
			if(NULL == thisFrame->m_hMDIClient)
			{
				return -1;
			}

			break;
		}
    case WM_NCDESTROY:
		{
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
			case ID_FILE_OPEN:
				{
					thisFrame->OnFileOpen(HIWORD(wParam), LOWORD(wParam), (HWND)lParam, bHandled);
					break;
				}
			case ID_FILE_SAVE:
				{
					thisFrame->OnFileSave(HIWORD(wParam), LOWORD(wParam), (HWND)lParam, bHandled);
					break;
				}
			case ID_FILE_SAVE_AS:
				{
					thisFrame->OnFileSaveAs(HIWORD(wParam), LOWORD(wParam), (HWND)lParam, bHandled);
					break;
				}
			case ID_FILE_EXIT:
				{
					thisFrame->OnExit(thisFrame->m_hWnd, HIWORD(wParam), LOWORD(wParam), (HWND)lParam, bHandled);
					break;
				}
			case ID_EDIT_FIND:
				{
					if(__nullptr == s_hwndFindDlg)
					{
						InitFIND();
						s_hwndFindDlg = ::FindText(&s_FIND);
						DWORD dwCommDlgError = ::CommDlgExtendedError();
					}
					else
					{
						::SetFocus(s_hwndFindDlg);
					}
					break;
				}
			case ID_EDIT_REPEAT:
				{
					BOOL bHandled = TRUE;
					s_FIND.Flags |= FR_DOWN;
					thisFrame->ChildDispatch(false, ID_EDIT_FIND, LPARAM(&s_FIND), bHandled);
					break;
				}
			case ID_EDIT_REPEAT_UP:
				{
					BOOL bHandled = TRUE;
					s_FIND.Flags &= ~FR_DOWN;
					thisFrame->ChildDispatch(false, ID_EDIT_FIND, LPARAM(&s_FIND), bHandled);
					break;
				}
			case ID_EDIT_REPLACE:
				{
					if(__nullptr == s_hwndFindReplaceDlg)
					{
						InitFINDREPLACE();
						s_hwndFindReplaceDlg = ::ReplaceText(&s_FINDREPLACE);
						DWORD dwCommDlgError = ::CommDlgExtendedError();
					}
					else
					{
						::SetFocus(s_hwndFindReplaceDlg);
					}
					break;
				}
			case ID_EDIT_CLEAR:
			case ID_EDIT_CLEAR_ALL:
			case ID_EDIT_COPY:
			case ID_EDIT_CUT:
			case ID_EDIT_PASTE:
			case ID_EDIT_PASTE_LINK:
			case ID_EDIT_PASTE_SPECIAL:
			case ID_EDIT_SELECT_ALL:
			case ID_EDIT_UNDO:
			case ID_EDIT_REDO:
				{
					WORD wCommand = LOWORD(wParam);
					thisFrame->ChildDispatch(0, wCommand, lParam, bHandled);
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
			case ID_FILE_MRU_FIRST:
			case ID_FILE_MRU_FILE2:
			case ID_FILE_MRU_FILE3:
			case ID_FILE_MRU_FILE4:
			case ID_FILE_MRU_FILE5:
			case ID_FILE_MRU_FILE6:
			case ID_FILE_MRU_FILE7:
			case ID_FILE_MRU_FILE8:
			case ID_FILE_MRU_FILE9:
			case ID_FILE_MRU_FILE10:
			case ID_FILE_MRU_FILE11:
			case ID_FILE_MRU_FILE12:
			case ID_FILE_MRU_FILE13:
			case ID_FILE_MRU_FILE14:
			case ID_FILE_MRU_FILE15:
			case ID_FILE_MRU_LAST:
				{
					thisFrame->OnOpenRecentFile(HIWORD(wParam), LOWORD(wParam), (HWND)lParam, bHandled);
				}
			}

			break;
		}
	default:
		{
			if(s_FindReplaceMsg == uMsg)
			{
				LPFINDREPLACE pFindReplace = LPFINDREPLACE(lParam);
				if(FR_DIALOGTERM & pFindReplace->Flags)
				{
					if(__nullptr == pFindReplace->lpstrReplaceWith)
					{
						s_hwndFindDlg = __nullptr;
					}
					else
					{
						s_hwndFindReplaceDlg = __nullptr;
					}

					return 0;
				}

				BOOL bHandled = TRUE;
				LRESULT lResult = 0;
				if(FR_FINDNEXT & pFindReplace->Flags)
				{
					thisFrame->ChildDispatch(false, ID_EDIT_FIND, LPARAM(pFindReplace), bHandled);
				}
				else if(FR_REPLACE & pFindReplace->Flags)
				{
					thisFrame->ChildDispatch(true, ID_EDIT_FIND, LPARAM(pFindReplace), bHandled);
				}
				else if(FR_REPLACEALL & pFindReplace->Flags)
				{
					do
					{
						lResult = thisFrame->ChildDispatch(true, ID_EDIT_FIND, LPARAM(pFindReplace), bHandled);
					}while(1 == lResult);
				}
				return 0;
			}
		}
	}

	return ::DefFrameProc(thisFrame->m_hWnd, (NULL == thisFrame ? NULL : thisFrame->m_hMDIClient), uMsg, wParam, lParam);
}

LRESULT CMultiRichFrame::ChildDispatch(WORD wParamHi, WORD wCommand, LPARAM lParam, BOOL& bHandled)
{
	LRESULT lr = 0;
	HWND hwndActive = HWND(::SendMessage(m_hMDIClient, WM_MDIGETACTIVE, 0, 0));
	if(__nullptr != hwndActive)
	{
		WPARAM wParam = MAKEWPARAM(wCommand, wParamHi);
		lr = ::SendMessage(hwndActive, WM_COMMAND, wParam, lParam);
	}

	return lr;
}

UINT_PTR CALLBACK CMultiRichFrame::FindHookProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	UINT_PTR uiReturn = 0U;

	switch(uMsg)
	{
	case WM_INITDIALOG:
		uiReturn = 1U;
		break;
	case WM_ACTIVATE:
		if(0 == wParam)
		{
			s_hwndCurrentDlg = __nullptr;
		}
		else
		{
			s_hwndCurrentDlg = hDlg;
		}
		uiReturn = 0U;
		break;
	}
	return uiReturn;
}

UINT_PTR CALLBACK CMultiRichFrame::FindReplaceHookProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	UINT_PTR uiReturn = 0U;

	switch(uMsg)
	{
	case WM_INITDIALOG:
		uiReturn = 1U;
		break;
	case WM_ACTIVATE:
		if(0 == wParam)
		{
			s_hwndCurrentDlg = __nullptr;
		}
		else
		{
			s_hwndCurrentDlg = hDlg;
		}
		uiReturn = 0U;
		break;
	}
	return uiReturn;
}

HRESULT CMultiRichModule::PreMessageLoop(int nCmdShow) throw()
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

	m_pFrame = new(std::nothrow) CMultiRichFrame;
	if(NULL == m_pFrame)
	{
		__super::PostMessageLoop();
		return E_OUTOFMEMORY;
	}
	HMODULE hMsftEdit = ::LoadLibraryW(L"Msftedit.dll");
	RECT rcPos = {CW_USEDEFAULT, 0, 0, 0};//CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT};
	HMENU hMainFrameMenu = ::LoadMenu(GetModuleHINSTANCE(), MAKEINTRESOURCE(IDR_MAINFRAME));
	HMENU hPregnantMenu = ::LoadMenu(GetModuleHINSTANCE(), MAKEINTRESOURCE(IDR_TEXTTYPE));
	HICON hIcon = ::LoadIcon(GetModuleHINSTANCE(), MAKEINTRESOURCE(IDI_MULTIRICH));
	CString sTitle;
	BOOL bStringLoaded = sTitle.LoadStringW(IDS_APP_TITLE);
	m_pFrame->GetWndClassInfo().m_wc.hIcon = hIcon;
	HWND hwndFrame = m_pFrame->Create(0, rcPos, sTitle, 0, 0, hMainFrameMenu);//, m_pFrame);
	if(__nullptr == hwndFrame)
	{
		return E_FAIL;
	}
	CMultiRichFrame::s_hwndThisFrame = hwndFrame;
	m_pFrame->m_hMainFrameMenu = hMainFrameMenu;
	m_pFrame->m_hPregnantMenu = hPregnantMenu;
	m_pFrame->m_hChildIcon = ::LoadIcon(GetModuleHINSTANCE(), MAKEINTRESOURCE(IDI_SMALL));

	m_pFrame->ShowWindow(nCmdShow);

	m_hAccelTable = LoadAccelerators(GetModuleHINSTANCE(), MAKEINTRESOURCE(IDC_MULTIRICH));
	return hResult;
}

void CMultiRichModule::RunMessageLoop() throw()
{
	MSG msg = {};
	while((GetMessage(&msg, 0, 0, 0) > 0))//WM_QUIT != msg.message
	{
		if(0 == ::TranslateMDISysAccel(m_pFrame->m_hMDIClient, &msg))
		{
			if(0 == ::TranslateAccelerator(m_pFrame->m_hWnd, m_hAccelTable, &msg))
			{
				if((__nullptr == CMultiRichFrame::s_hwndCurrentDlg) || !::IsDialogMessage(CMultiRichFrame::s_hwndCurrentDlg, &msg))
				{
					::TranslateMessage(&msg);
					::DispatchMessage(&msg);
				}
			}
		}
	}
}

HRESULT CMultiRichModule::PostMessageLoop() throw()
{
	HRESULT hr = __super::PostMessageLoop();
	if(NULL != m_pFrame)
	{
		delete m_pFrame;
		m_pFrame = NULL;
	}
	return hr;
}


CMultiRichModule _AtlModule;



//
extern "C" int WINAPI _tWinMain(HINSTANCE /*hInstance*/, HINSTANCE /*hPrevInstance*/, 
								LPTSTR /*lpCmdLine*/, int nShowCmd)
{
	return _AtlModule.WinMain(nShowCmd);
}

