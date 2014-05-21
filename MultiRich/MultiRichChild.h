#pragma once
class CATLRichEdit;
class CMultiRichFrame;
class CMultiRichChild : public CWindowImpl<CMultiRichChild, CWindow, CMDIChildWinTraits>
{
public:
	HWND			m_hwndFrame;
	HWND			m_hwndEdit;
	CString			m_sOriginalTitle;
	CATLRichEdit*		m_pATLRichEdit;
	CMultiRichFrame*	m_pMDIFrame;
	bool			m_bActivated;
	DECLARE_WND_CLASS(_T("MULTIRICHCHILD"))
	BEGIN_MSG_MAP(CMultiRichChild)
		COMMAND_ID_HANDLER(ID_EDIT_CLEAR, OnEditClear)
		COMMAND_ID_HANDLER(ID_EDIT_FIND, OnEditFind)
		COMMAND_ID_HANDLER(ID_EDIT_SELECT_ALL, OnEditSelectAll)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_MDIACTIVATE, OnMDIActivate)
		MESSAGE_HANDLER(WM_SETFOCUS, OnSetFocus)
		MESSAGE_HANDLER(WM_WINDOWPOSCHANGED, OnWindowPosChanged)
	END_MSG_MAP()
	CMultiRichChild();
	~CMultiRichChild();
	LRESULT OnEditClear(WORD /*wHiParam*/, WORD /*wLoParam*/, HWND hwnd, BOOL& /*bHandled*/);
	LRESULT OnEditFind(WORD /*wHiParam*/, WORD /*wLoParam*/, HWND hwnd, BOOL& /*bHandled*/);
	LRESULT OnEditSelectAll(WORD /*wHiParam*/, WORD /*wLoParam*/, HWND hwnd, BOOL& /*bHandled*/);
	LRESULT OnCreate(UINT /*nMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnMDIActivate(UINT /*nMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnSetFocus(UINT /*nMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnWindowPosChanged(UINT , WPARAM , LPARAM , BOOL& );
	VOID OnFinalMessage(HWND /*hwnd*/);
};

