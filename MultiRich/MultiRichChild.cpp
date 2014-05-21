#include "StdAfx.h"
#include "ATLRichEdit.h"
#include "MultiRichChild.h"
#include "MultiRich.h"


CMultiRichChild::CMultiRichChild(void) : 
	m_hwndFrame(__nullptr),
	m_hwndEdit(__nullptr),
	m_pATLRichEdit(__nullptr),
	m_pMDIFrame(__nullptr),
	m_bActivated(false)
{
}


CMultiRichChild::~CMultiRichChild(void)
{
}

LRESULT CMultiRichChild::OnEditClear(WORD /*wHiParam*/, WORD /*wLoParam*/, HWND hwnd, BOOL& /*bHandled*/)
{
	return 0;
}

LRESULT CMultiRichChild::OnEditSelectAll(WORD /*wHiParam*/, WORD /*wLoParam*/, HWND hwnd, BOOL& /*bHandled*/)
{
	::SendMessage(m_hwndEdit, EM_SETSEL, WPARAM(0), LPARAM(-1));
	//::SetFocus(m_hwndEdit);
	return 0;
}

LRESULT CMultiRichChild::OnEditFind(WORD wHiParam, WORD wLoParam, HWND hwnd, BOOL& bHandled)
{
	LPFINDREPLACE pFindReplace = LPFINDREPLACE(hwnd);
	if(__nullptr != pFindReplace)
	{
		bool bReplace = (0 == wHiParam) ? false : true;
		HRESULT hr = m_pATLRichEdit->Find(pFindReplace, bReplace);
		if(S_OK != hr)
		{
			bHandled = FALSE;
		}
		else
		{
			return 1;
		}
	}

	return 0;
}

LRESULT CMultiRichChild::OnCreate(UINT /*nMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled)
{
	m_hwndFrame = 0;
	m_hwndEdit = 0;
	bHandled = FALSE;

	CREATESTRUCT* pCreate = (CREATESTRUCT*)lParam;
	if(__nullptr != pCreate)
	{
		MDICREATESTRUCT* pMDICreate = (MDICREATESTRUCT*)pCreate->lpCreateParams;
		if(__nullptr != pMDICreate)
		{
			m_pMDIFrame = (CMultiRichFrame*)pMDICreate->lParam;
			if(__nullptr == m_pMDIFrame)
			{
				return -1;
			}
			m_hwndFrame = m_pMDIFrame->m_hWnd;
			m_sOriginalTitle = pMDICreate->szTitle;

			CATLRichEdit* pATLEdit = new CATLRichEdit;
			if(__nullptr != pATLEdit)
			{
				m_hwndEdit = pATLEdit->Create(m_hWnd);
				if(0 == m_hwndEdit)
				{
					delete pATLEdit;
					::TaskDialog(__nullptr, __nullptr, __nullptr, L"Edit creation failed!", __nullptr, TDCBF_OK_BUTTON, TD_INFORMATION_ICON, __nullptr);
				}
				else
				{
					m_pATLRichEdit = pATLEdit;
					HRESULT hr = pATLEdit->OpenFile(m_sOriginalTitle, GENERIC_READ, FILE_SHARE_READ|FILE_SHARE_WRITE, OPEN_EXISTING);
				}
			}
		}
	}

	return 0;
}

LRESULT CMultiRichChild::OnMDIActivate(UINT /*nMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if(HWND(lParam) == m_hWnd)
	{
		m_bActivated = true;
		::SetFocus(m_hwndEdit);
	}
	else if(HWND(wParam) == m_hWnd)
	{
		m_bActivated = false;
	}
	return 0;
}

LRESULT CMultiRichChild::OnSetFocus(UINT /*nMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	::SetFocus(m_hwndEdit);
	return 0;
}

LRESULT CMultiRichChild::OnWindowPosChanged(UINT , WPARAM , LPARAM , BOOL& bHandled)
{
	bHandled = FALSE;
	RECT rc;
	GetClientRect(&rc);
	::SetWindowPos(m_hwndEdit, HWND_TOP, 0, 0, rc.right, rc.bottom, 0);
	return 0;
}

VOID CMultiRichChild::OnFinalMessage(HWND /*hwnd*/)
{
	WPARAM wParam = (WPARAM)((LPCTSTR)m_sOriginalTitle);
	BOOL bHandled = FALSE;
	//::SendMessage(m_hwndFrame, WM_MDIDESTROY, wParam, (LPARAM)this);
	//Now that we have a pFrame no need to send, just call.
	m_pMDIFrame->OnMDIDestroy(WM_MDIDESTROY, wParam, LPARAM(this), bHandled);
	delete this;
}