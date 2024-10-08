//ATLXAML.h
//typedef CWinTraits<WS_CHILD |WS_VISIBLE|WS_CLIPCHILDREN|WS_CLIPSIBLINGS, 0> CControlWinTraits;
//typedef CWinTraits<WS_OVERLAPPEDWINDOW|WS_CLIPCHILDREN|WS_CLIPSIBLINGS, WS_EX_APPWINDOW|WS_EX_WINDOWEDGE>	CFrameWinTraits;
//typedef CWinTraits<WS_OVERLAPPEDWINDOW|WS_CHILD|WS_VISIBLE|WS_CLIPCHILDREN|WS_CLIPSIBLINGS, WS_EX_MDICHILD> CMDIChildWinTraits;
#pragma once
//Below not used here but for testing
typedef CWinTraits<WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN, WS_EX_CLIENTEDGE> CChildWinTraits;
typedef CWinTraits< WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_VISIBLE, 0> CDxFrameWinTraits;
//Above not used here but for testing

class CATLXAMLFrame : public CWindowImpl<CATLXAMLFrame, CWindow, CFrameWinTraits>
{
public:
    CATLXAMLFrame() :
        m_hMDIClient(0),
        m_hNoPaneMenu(__nullptr),
        m_hPaneMenu(__nullptr),
        m_hSplitCursor(__nullptr),
        m_bBinHereDoneThat(false)
    {
        (void)m_sUntitled.LoadStringW(IDS_UNTITLED);
    }
    HWND m_hMDIClient;
    HMENU m_hPaneMenu;
    HMENU m_hNoPaneMenu;
    HCURSOR m_hSplitCursor;
    bool m_bBinHereDoneThat;
    CString m_sUntitled;
    std::vector<CATLXAMLChild*> m_vecMDIChildren;
    std::vector<CString> m_vecTitlesToRecycle;
    DECLARE_WND_CLASS(_T("ATLXAMLFrame"))
    BEGIN_MSG_MAP(CATLXAMLFrame)
        //Empty map is neccessary to define
        //ProcessWindowMessage(..) declared somewhere in the base
    END_MSG_MAP()
    static LRESULT CALLBACK FrameWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    LRESULT OnInitMenu(UINT, WPARAM, LPARAM, BOOL&);
    LRESULT OnSize(UINT, WPARAM, LPARAM, BOOL&);
    LRESULT OnAboutBox(HWND hWnd, WORD, WORD, HWND, BOOL&);
    LRESULT OnFileNew(WORD, WORD, HWND, BOOL&);
    WNDPROC GetWindowProc()
    {
        return FrameWindowProc;
    }
    //LRESULT OnMouseHover(UINT /*nMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
    LRESULT OnMDIDestroy(UINT /*nMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
    LRESULT OnExit(HWND hWnd, WORD, WORD, HWND, BOOL&)
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

class CATLXAMLModule : public ATL::CAtlExeModuleT<CATLXAMLModule>
{
public:
    CATLXAMLModule() : m_pFrame(NULL), m_hAccelTable(0){};
    ~CATLXAMLModule(){};
    CATLXAMLFrame* m_pFrame;
    HACCEL m_hAccelTable;
    HRESULT PreMessageLoop(int nCmdShow) throw();
    void RunMessageLoop() throw();
    HRESULT PostMessageLoop() throw();
};

