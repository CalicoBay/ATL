#pragma once
class CATLMDIFrame;


class CATLMDIChild : public CWindowImpl<CATLMDIChild, CWindow, CMDIChildWinTraits>
{
	// Xaml island state.
	struct WindowInfo
	{
		winrt::Microsoft::UI::Xaml::Hosting::DesktopWindowXamlSource DesktopWindowXamlSource{ nullptr };
		winrt::event_token TakeFocusRequestedToken{};
		HWND LastFocusedWindow{ NULL };
	};

public:
	CATLMDIChild();
	~CATLMDIChild();
	HWND m_hwndFrame;
	HWND m_hwndXamlIsland;
	CString m_sOriginalTitle;
	CString m_sContent;
   CString m_sStaticContent;
   CATLMDIFrame* m_pMDIFrame;
	bool m_bActivated;
	RECT m_RectStatic;
   HCURSOR m_hSplitCursor;
   CContainedWindow  m_Static;
	DECLARE_WND_CLASS(_T("ATLMDIChild"))
	BEGIN_MSG_MAP(CATLMDIChild)
		COMMAND_ID_HANDLER(ID_CLEAR_TEXT, OnClearText)
		COMMAND_ID_HANDLER(ID_SET_TEXT, OnSetText)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_NCDESTROY, OnNcDestroy)
		MESSAGE_HANDLER(WM_MDIACTIVATE, OnMDIActivate)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
      //ALT_MSG_MAP(1) // I think these were necessary when CContainedWindow had window class "Edit"
      //ALT_MSG_MAP(2)
	END_MSG_MAP()
	LRESULT OnClearText(WORD /*wHiParam*/, WORD /*wLoParam*/, HWND hwnd, BOOL& /*bHandled*/);//WORD, WORD, HWND, BOOL&
	LRESULT OnSetText(WORD /*wHiParam*/, WORD /*wLoParam*/, HWND hwnd, BOOL& /*bHandled*/);
	LRESULT OnCreate(UINT /*nMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnMDIActivate(UINT /*nMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/);
	LRESULT OnNcDestroy(UINT /*nMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnSize(UINT /*nMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
   VOID OnFinalMessage(HWND /*hwnd*/);
	WindowInfo* m_pWindowInfo;
	winrt::ATLMDI::MainPage m_MainPage{ __nullptr };
};

