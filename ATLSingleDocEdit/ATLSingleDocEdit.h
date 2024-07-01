#pragma once
//|ES_LEFT
class CATLEdit : public CWindowImpl<CATLEdit, CWindow, CWinTraits<WS_CHILD|WS_VISIBLE|WS_VSCROLL|ES_MULTILINE|ES_AUTOVSCROLL>>
{
public:
	CATLEdit() :
		m_numZoom(0),
		m_denZoom(0){}
	DECLARE_WND_SUPERCLASS(_T("CATLEdit"), _T("EDIT"))
	BEGIN_MSG_MAP(CATLEdit)
		MESSAGE_HANDLER(WM_KEYDOWN, OnKeyDown)
	END_MSG_MAP()
	LRESULT OnKeyDown(UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
private:
	WPARAM m_numZoom;
	LPARAM m_denZoom;
};

class CATLSingleFrame : public CWindowImpl<CATLSingleFrame, CWindow, CFrameWinTraits>
{
public:
	CATLSingleFrame(): m_hwndEdit(0){BOOL bReturn = m_sUntitled.LoadStringW(IDS_UNTITLED);}
	HWND m_hwndEdit;
	CString m_sUntitled;
	DECLARE_WND_CLASS(_T("ATLSINGLEDOCEDIT"))
	BEGIN_MSG_MAP(CATLSingleFrame)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
      MESSAGE_HANDLER(WM_SIZE, OnSize)
		COMMAND_ID_HANDLER(IDM_ABOUT, OnAboutBox)
		COMMAND_ID_HANDLER(IDM_EXIT, OnExit)
	END_MSG_MAP()
	LRESULT OnCreate(UINT , WPARAM , LPARAM , BOOL& );
	LRESULT OnSize(UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnAboutBox(WORD , WORD , HWND , BOOL& );
	//LRESULT OnFileNew(WORD, WORD, HWND, BOOL&);
	//LRESULT OnMDIDestroy(UINT /*nMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnExit(WORD , WORD , HWND , BOOL& )
	{
		DestroyWindow();
		return 0;
	}
	void OnFinalMessage(HWND /*hWnd*/)
	{
		::PostQuitMessage(0);
	}
};

class CATLSingleDocModule : public ATL::CAtlExeModuleT<CATLSingleDocModule>
{
public:
	CATLSingleDocModule(): m_pFrame(NULL), m_hAccelTable(0){};
	~CATLSingleDocModule(){};
	CATLSingleFrame* m_pFrame;
	HACCEL m_hAccelTable;
	HRESULT PreMessageLoop(int nCmdShow) throw();
	void RunMessageLoop() throw();
	HRESULT PostMessageLoop() throw();
};

