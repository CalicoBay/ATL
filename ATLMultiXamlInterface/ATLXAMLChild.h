#pragma once
class CATLXAMLFrame;

class CATLXAMLChild : public CWindowImpl<CATLXAMLChild, CWindow, CMDIChildWinTraits>
{
public:
    CATLXAMLChild();
    ~CATLXAMLChild();
    HWND					m_hwndFrame;
    CString m_sOriginalTitle;
    CString	m_sContent;
    CString	m_sStaticContent;
    CString m_sFilePath;
    CATLXAMLFrame* m_pMDIFrame;
    RECT m_RectStatic, m_RectEdit;
    HCURSOR m_hSplitCursor;
    HCURSOR m_hArrowCursor;
    CContainedWindowT<ATLControls::CBOMEdit, CWinTraitsOR<WS_CHILD | WS_BORDER | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL | ES_MULTILINE>>  m_Edit;
    CContainedWindow  m_Static;
    DECLARE_WND_CLASS(_T("ATLXAMLChild"))
    BEGIN_MSG_MAP(CATLXAMLChild)
        COMMAND_ID_HANDLER(ID_CLEAR_TEXT, OnClearText)
        COMMAND_ID_HANDLER(ID_SET_TEXT, OnSetText)
        COMMAND_ID_HANDLER(ID_FILE_CLOSE, OnFileClose)
        COMMAND_ID_HANDLER(ID_FILE_OPEN, OnFileOpen)
        COMMAND_ID_HANDLER(ID_FILE_SAVE, OnFileSave)
        COMMAND_ID_HANDLER(ID_FILE_SAVEAS, OnFileSaveAs)
        MESSAGE_HANDLER(WM_CREATE, OnCreate)
        MESSAGE_HANDLER(WM_PAINT, OnPaint)
        MESSAGE_HANDLER(WM_MOUSEMOVE, OnMouseMove)
        MESSAGE_HANDLER(WM_LBUTTONDOWN, OnLButtonDown)
        MESSAGE_HANDLER(WM_LBUTTONUP, OnLButtonUp)
        MESSAGE_HANDLER(WM_SIZE, OnSize)
        ALT_MSG_MAP(1)
        ALT_MSG_MAP(2)
    END_MSG_MAP()
    LRESULT OnClearText(WORD /*wHiParam*/, WORD /*wLoParam*/, HWND hwnd, BOOL& /*bHandled*/);//WORD, WORD, HWND, BOOL&
    LRESULT OnSetText(WORD /*wHiParam*/, WORD /*wLoParam*/, HWND hwnd, BOOL& /*bHandled*/);
    LRESULT OnCreate(UINT /*nMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
    LRESULT OnPaint(UINT /*nMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
    LRESULT OnMouseMove(UINT /*nMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT OnLButtonDown(UINT /*nMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT OnLButtonUp(UINT /*nMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT OnSize(UINT /*nMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
    LRESULT OnFileClose(WORD wHiParam, WORD wLoParam, HWND hwnd, BOOL& bHandled);
    LRESULT OnFileOpen(WORD, WORD, HWND, BOOL&);
    LRESULT OnFileSave(WORD, WORD, HWND, BOOL& bHandled);
    LRESULT OnFileSaveAs(WORD, WORD, HWND, BOOL& bHandled);
    VOID OnFinalMessage(HWND /*hwnd*/);

};

