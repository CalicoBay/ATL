#pragma once


class CDxMain : public CWindowImpl<CDxMain, CWindow, CWinTraits<WS_OVERLAPPEDWINDOW|WS_CLIPSIBLINGS>>
{
public:
	CDxMain();
	~CDxMain();
	DECLARE_WND_CLASS(_T("DXMain"))
	HRESULT Initialize();
	VOID PaintCurrentTab();
	void OnFinalMessage(HWND /*hWnd*/)
	{
		::PostQuitMessage(0);
	}
protected:
	BEGIN_MSG_MAP(CDxMain)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_GETMINMAXINFO, OnGetMinMaxInfo)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
      MESSAGE_HANDLER(WM_SETFOCUS, OnSetFocus)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
		NOTIFY_CODE_HANDLER(TCN_SELCHANGE, OnTabSelChange)
		COMMAND_ID_HANDLER(IDM_ABOUT, OnAboutBox)
		COMMAND_ID_HANDLER(IDM_EXIT, OnExit)
	END_MSG_MAP()
	LRESULT OnCreate(UINT /*nMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnGetMinMaxInfo(UINT /*nMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnPaint(UINT /*nMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnSize(UINT /*nMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
   LRESULT OnSetFocus(UINT /*nMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
   {
      //::SendMessage(m_hWndCurrentTab, WM_SETFOCUS, WPARAM(m_hWnd), LPARAM(0));
      ::SetFocus(m_hWndCurrentTab);
      return 0;
   }
	LRESULT OnTabSelChange(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);
	LRESULT OnAboutBox(WORD , WORD , HWND , BOOL& )
	{
		::DialogBoxParam(::GetModuleHINSTANCE(), MAKEINTRESOURCE(IDD_ABOUTBOX), m_hWnd, About, (LPARAM)this);
		return 0;
	}
	LRESULT OnExit(WORD , WORD , HWND , BOOL& )
	{
		m_TabControl.DestroyWindow();
		m_TabControl.Detach();
		DestroyWindow();
		return 0;
	}
	
	static INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
	CWindow m_TabControl;
	std::vector<CWindow*> m_vecTabWindows;
	static LONG m_vecTabCapacity;
	static LONG m_cTabCount;
	HWND m_hWndCurrentTab;
};

