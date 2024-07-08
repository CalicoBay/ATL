#pragma once
class CATLMDIFrame;

class CATLMDIChild : public CWindowImpl<CATLMDIChild, CWindow, CMDIChildWinTraits>
{
public:
	CATLMDIChild();
	~CATLMDIChild();
	HWND					m_hwndFrame;
	CString					m_sOriginalTitle;
	CString					m_sContent;
   CString					m_sStaticContent;
   CATLMDIFrame*			m_pMDIFrame;
   RECT m_RectStatic;
   HCURSOR m_hSplitCursor;
   CContainedWindow  m_Static;
	DECLARE_WND_CLASS(_T("ATLMDIChild"))
	BEGIN_MSG_MAP(CATLMDIChild)
		COMMAND_ID_HANDLER(ID_CLEAR_TEXT, OnClearText)
		COMMAND_ID_HANDLER(ID_SET_TEXT, OnSetText)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
      MESSAGE_HANDLER(WM_SIZE, OnSize)
      ALT_MSG_MAP(1)
      ALT_MSG_MAP(2)
	END_MSG_MAP()
	LRESULT OnClearText(WORD /*wHiParam*/, WORD /*wLoParam*/, HWND hwnd, BOOL& /*bHandled*/);//WORD, WORD, HWND, BOOL&
	LRESULT OnSetText(WORD /*wHiParam*/, WORD /*wLoParam*/, HWND hwnd, BOOL& /*bHandled*/);
	LRESULT OnCreate(UINT /*nMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
   LRESULT OnSize(UINT /*nMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
   VOID OnFinalMessage(HWND /*hwnd*/);

};

