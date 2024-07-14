//ATLMDI.h
//typedef CWinTraits<WS_CHILD |WS_VISIBLE|WS_CLIPCHILDREN|WS_CLIPSIBLINGS, 0> CControlWinTraits;
//typedef CWinTraits<WS_OVERLAPPEDWINDOW|WS_CLIPCHILDREN|WS_CLIPSIBLINGS, WS_EX_APPWINDOW|WS_EX_WINDOWEDGE>	CFrameWinTraits;
//typedef CWinTraits<WS_OVERLAPPEDWINDOW|WS_CHILD|WS_VISIBLE|WS_CLIPCHILDREN|WS_CLIPSIBLINGS, WS_EX_MDICHILD> CMDIChildWinTraits;
#pragma once
class CATLMDIChild;

//Below not used here but for testing
typedef CWinTraits<WS_CHILD|WS_VISIBLE|WS_CLIPCHILDREN, WS_EX_CLIENTEDGE> CChildWinTraits;
typedef CWinTraits< WS_OVERLAPPEDWINDOW|WS_CLIPCHILDREN|WS_VISIBLE, 0> CDxFrameWinTraits;
//Above not used here but for testing

class CATLMDIFrame : public CWindowImpl<CATLMDIFrame, CWindow, CFrameWinTraits>
{
public:
	CATLMDIFrame():
      m_hMDIClient(0),
      m_hNoPaneMenu(__nullptr),
      m_hPaneMenu(__nullptr),
      m_hSplitCursor(__nullptr)
   {
		m_sUntitled = _T("Web Site ");//(void)m_sUntitled.LoadStringW(IDS_UNTITLED);
   }
	HWND m_hMDIClient;
	HMENU m_hPaneMenu;
	HMENU m_hNoPaneMenu;
   HCURSOR m_hSplitCursor;
	CString m_sUntitled;
	std::vector<CATLMDIChild*> m_vecMDIChildren;
	std::vector<CString> m_vecTitlesToRecycle;
	DECLARE_WND_CLASS(_T("ATLMDIFrame"))
	BEGIN_MSG_MAP(CATLMDIFrame)
		//Empty map is neccessary to define
		//ProcessWindowMessage(..) declared somewhere in the base
	END_MSG_MAP()
	static LRESULT CALLBACK FrameWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnInitMenu(UINT , WPARAM , LPARAM , BOOL& );
	LRESULT OnSize(UINT , WPARAM , LPARAM , BOOL& );
	LRESULT OnAboutBox(HWND hWnd, WORD , WORD , HWND , BOOL& );
	LRESULT OnFileNew(WORD, WORD, HWND, BOOL&);
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
	VOID CommandDispatch(WORD wCommand, HWND hwnd, BOOL& bHandled);
};

class CATLMDIModule : public ATL::CAtlExeModuleT<CATLMDIModule>
{
public:
	CATLMDIModule(): m_pFrame(NULL), m_hAccelTable(0){};
	~CATLMDIModule(){};
	CATLMDIFrame* m_pFrame;
	HACCEL m_hAccelTable;
	HRESULT PreMessageLoop(int nCmdShow) throw();
	void RunMessageLoop() throw();
	HRESULT PostMessageLoop() throw();
protected:
	winrt::Microsoft::UI::Dispatching::DispatcherQueueController m_DispatcherQueueController{__nullptr};
	// See: https://learn.microsoft.com/en-us/windows/uwp/cpp-and-winrt-apis/consume-apis
	// Search locallly for std::nullptr_t
	winrt::Microsoft::UI::Xaml::Application m_XamlApp{__nullptr};
};

