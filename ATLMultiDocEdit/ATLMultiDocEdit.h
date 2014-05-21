#pragma once

class CATLMultiFrame : public CWindowImpl<CATLMultiFrame, CWindow, CFrameWinTraits>
{
public:
	CATLMultiFrame();

	HWND m_hMDIClient;
	HMENU m_hMainFrameMenu;
	HMENU m_hPregnantMenu;
	HICON m_hChildIcon;
	CString m_sUntitled;
	static TCHAR s_tszFindWhat[0x80];
	static TCHAR s_tszReplaceWhat[0x80];
	static TCHAR s_tszReplaceWith[0x80];
	static UINT s_FindReplaceMsg;
	static HWND s_hwndThisFrame;
	static HWND s_hwndFindDlg;
	static HWND s_hwndFindReplaceDlg;
	static HWND s_hwndCurrentDlg;
	static FINDREPLACE s_FIND;
	static FINDREPLACE s_FINDREPLACE;
	std::vector<CATLMultiEditChild*> m_vecMDIChildren;
	std::vector<CString> m_vecTitlesToRecycle;
	DECLARE_WND_CLASS(_T("ATLMULTIDOCEDIT"))
	BEGIN_MSG_MAP(CATLMultiFrame)
		//Empty map is neccessary to define
		//ProcessWindowMessage(..) declared somewhere in the base
	END_MSG_MAP()
	static LRESULT CALLBACK FrameWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	static UINT_PTR CALLBACK FindHookProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
	static UINT_PTR CALLBACK FindReplaceHookProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnInitMenu(UINT , WPARAM , LPARAM , BOOL& );
	LRESULT OnSize(UINT , WPARAM , LPARAM , BOOL& );
	LRESULT OnAboutBox(HWND hWnd, WORD , WORD , HWND , BOOL& );
	LRESULT OnFileNew(WORD, WORD, HWND, BOOL&);
	LRESULT OnFileOpen(WORD, WORD, HWND, BOOL&);
	LRESULT OnFileSave(WORD, WORD, HWND, BOOL& bHandled);
	LRESULT OnFileSaveAs(WORD, WORD, HWND, BOOL&);
	WNDPROC GetWindowProc()
	{
		return FrameWindowProc;
	}
	LRESULT OnMDIDestroy(UINT /*nMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnExit(HWND hWnd, WORD , WORD , HWND , BOOL& )
	{
		::DestroyWindow(hWnd);
		return 0;
	}
	void OnFinalMessage(HWND /*hWnd*/)
	{
		::PostQuitMessage(0);
	}
protected:
	UINT m_NewCount;
	VOID ChildDispatch(WORD wCommand, LPARAM lParam, BOOL& bHandled);
	static VOID InitFIND();
	static VOID InitFINDREPLACE();
};

class CATLMultiDocModule : public ATL::CAtlExeModuleT<CATLMultiDocModule>
{
public:
	CATLMultiDocModule(): m_pFrame(NULL), m_hAccelTable(0){};
	~CATLMultiDocModule(){};
	CATLMultiFrame* m_pFrame;
	HACCEL m_hAccelTable;
	HRESULT PreMessageLoop(int nCmdShow) throw();
	void RunMessageLoop() throw();
	HRESULT PostMessageLoop() throw();
};

