#pragma once

class CMultiRichFrame : public CWindowImpl<CMultiRichFrame, CWindow, CFrameWinTraits>
{
public:
	CMultiRichFrame();

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
	std::vector<CMultiRichChild*> m_vecMDIChildren;
	std::vector<CString> m_vecTitlesToRecycle;
	std::vector<CString> m_vecMRUs;
	DECLARE_WND_CLASS(_T("MULTIRICHEDIT"))
	BEGIN_MSG_MAP(CMultiRichFrame)
		//Empty map is neccessary to define
		//ProcessWindowMessage(..) declared somewhere in the base
		COMMAND_RANGE_HANDLER(ID_FILE_MRU_FIRST, ID_FILE_MRU_LAST, OnOpenRecentFile)
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
	LRESULT ChildDispatch(WORD wParamHi, WORD wCommandLo, LPARAM lParam, BOOL& bHandled);
	LRESULT OnOpenRecentFile(WORD, WORD, HWND, BOOL&);
	static VOID InitFIND();
	static VOID InitFINDREPLACE();
	BOOL m_bStringLoaded;
};

class CMultiRichModule : public ATL::CAtlExeModuleT<CMultiRichModule>
{
public:
	CMultiRichModule(): m_pFrame(NULL), m_hAccelTable(0){};
	~CMultiRichModule(){};
	CMultiRichFrame* m_pFrame;
	HACCEL m_hAccelTable;
	HRESULT PreMessageLoop(int nCmdShow) throw();
	void RunMessageLoop() throw();
	HRESULT PostMessageLoop() throw();
};

