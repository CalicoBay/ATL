#include "stdafx.h"
#include "ATL-MDIChild.h"
#include "ATL-MDI.h"

static short begX = 0;
static short endX = 0;

CATLMDIChild::CATLMDIChild(void) : 
   m_Edit(_T("Edit"), this, 1),
   m_Static(_T("Edit"), this, 2),
   m_hwndFrame(0),
   m_pMDIFrame(__nullptr),
   m_hSplitCursor(__nullptr)
{
   ::memset(&m_RectStatic, 0, sizeof(RECT));
   ::memset(&m_RectEdit, 0, sizeof(RECT));
   m_hSplitCursor = ::LoadCursor(__nullptr, MAKEINTRESOURCE(IDC_SIZENS));
   m_sContent = _T("TODO: Something with this space!");
   m_sStaticContent = _T("HWND hwndStatic = m_Static.Create(m_hWnd, rcStatic, m_sStaticContent, WS_CHILD | WS_BORDER | WS_VISIBLE | WS_VSCROLL | ES_MULTILINE | ES_READONLY);");
}


CATLMDIChild::~CATLMDIChild(void)
{
}

LRESULT CATLMDIChild::OnCreate(UINT /*nMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	CREATESTRUCT* pCreate = (CREATESTRUCT*)lParam;
	if(__nullptr != pCreate)
	{
		MDICREATESTRUCT* pMDICreate = (MDICREATESTRUCT*)pCreate->lpCreateParams;
		if(__nullptr != pMDICreate)
		{
			m_pMDIFrame = (CATLMDIFrame*)pMDICreate->lParam;
			if(__nullptr == m_pMDIFrame)
			{
				return -1;
			}
			m_hwndFrame = m_pMDIFrame->m_hWnd;
			m_sOriginalTitle = pMDICreate->szTitle;
         RECT rcClient;
         GetClientRect(&rcClient);
         m_RectStatic.left = rcClient.left;
         m_RectStatic.top = rcClient.top;
         m_RectStatic.right = rcClient.right;
         m_RectStatic.bottom = (rcClient.bottom - rcClient.top) / 2;

         m_RectEdit.left = rcClient.left;
         m_RectEdit.top = m_RectStatic.bottom + 5;
         m_RectEdit.right = rcClient.right;
         m_RectEdit.bottom = rcClient.bottom;
         HWND hwndStatic = m_Static.Create(m_hWnd, m_RectStatic, m_sStaticContent, WS_CHILD | WS_BORDER | WS_VISIBLE | WS_VSCROLL | ES_MULTILINE | ES_READONLY);
         HWND hwndEdit = m_Edit.Create(m_hWnd, m_RectEdit, m_sContent, WS_CHILD | WS_BORDER | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL | ES_MULTILINE);
         //if(0 != hwndEdit)
         //{
         //   m_Edit.SetFocus();
         //}
		}
	}

	return 0;
}

LRESULT CATLMDIChild::OnClearText(WORD /*wHiParam*/, WORD /*wLoParam*/, HWND hwnd, BOOL& /*bHandled*/)
{
	m_sContent.Empty();
   m_Edit.SendMessage(EM_SETSEL, 0, -1);
   m_Edit.SendMessage(EM_REPLACESEL, FALSE, (LPARAM)m_sContent.GetString());
   //Invalidate();
	return 0;
}

LRESULT CATLMDIChild::OnSetText(WORD /*wHiParam*/, WORD /*wLoParam*/, HWND hwnd, BOOL& /*bHandled*/)
{
	m_sContent = _T("TODO: We need to get some text from a TaskDialog for instance.");
   m_Edit.SendMessage(EM_SETSEL, 0, -1);
   m_Edit.SendMessage(EM_REPLACESEL, FALSE, (LPARAM)m_sContent.GetString());
	//Invalidate();
	return 0;
}

LRESULT CATLMDIChild::OnPaint(UINT /*nMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	PAINTSTRUCT ps;
	HDC hdc = GetDC();
   RECT rectToPaint;
   HRGN rgnToPaint;
   rectToPaint.bottom = m_RectEdit.top;
   rectToPaint.left = m_RectStatic.left;
   rectToPaint.right = m_RectEdit.right;
   rectToPaint.top = m_RectStatic.bottom;
   rgnToPaint = ::CreateRectRgnIndirect(&rectToPaint);
	BeginPaint(&ps);
   ::FillRgn(hdc, rgnToPaint, GetSysColorBrush(COLOR_ACTIVECAPTION));
	EndPaint(&ps);

	return 0;
}

LRESULT CATLMDIChild::OnSize(UINT, WPARAM, LPARAM, BOOL&)
{
   RECT rcClient;
   GetClientRect(&rcClient);
   m_RectStatic.left = m_RectEdit.left = rcClient.left;
   m_RectStatic.right = m_RectEdit.right = rcClient.right;

   if(rcClient.bottom > (m_RectStatic.bottom + 150))
   {
      m_RectEdit.bottom = rcClient.bottom;
   }
   else
   {
      m_RectEdit.bottom = m_RectStatic.bottom + 150;
   }
   m_Static.SetWindowPos(HWND_TOP, &m_RectStatic, 0);
   m_Edit.SetWindowPos(HWND_TOP, &m_RectEdit, 0);
   return 0;
}

LRESULT CATLMDIChild::OnMouseMove(UINT /*nMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
   LRESULT lResult = 0;
   //SHORT xPos = GET_X_LPARAM(lParam);
   //SHORT yPos = GET_Y_LPARAM(lParam);
   //if(MK_LBUTTON & wParam)
   //{
   //   //TRACKMOUSEEVENT tme;
   //   //tme.cbSize = sizeof(tme);
   //   //tme.dwFlags = TME_LEAVE;
   //   //tme.hwndTrack = m_hWnd;
   //   //::SetCursor(m_hSplitCursor);
   //}
   ::SetCursor(m_hSplitCursor);
   return lResult;
}

LRESULT CATLMDIChild::OnLButtonDown(UINT /*nMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
   RECT rcClient;
   GetWindowRect(&rcClient);
   rcClient.top += 100;
   rcClient.bottom -= 150;
   ::ClipCursor(&rcClient);
   begX = GET_Y_LPARAM(lParam);
   ::SetCursor(m_hSplitCursor);
   ::SetCapture(m_hWnd);
   return 0;
}

LRESULT CATLMDIChild::OnLButtonUp(UINT /*nMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
   endX = GET_Y_LPARAM(lParam);
   short deltaX = endX - begX;
   m_RectStatic.bottom += deltaX;
   m_RectEdit.top += deltaX; 
   m_Static.SetWindowPos(HWND_TOP, &m_RectStatic, 0);
   m_Edit.SetWindowPos(HWND_TOP, &m_RectEdit, 0);
   ::ClipCursor(NULL);
   ::ReleaseCapture();
   return 0;
}

VOID CATLMDIChild::OnFinalMessage(HWND /*hwnd*/)
{
	WPARAM wParam = (WPARAM)((LPCTSTR)m_sOriginalTitle);
	BOOL bHandled = FALSE;
	//::SendMessage(m_hwndFrame, WM_MDIDESTROY, wParam, (LPARAM)this);
	//Now that we have a pFrame no need to send, just call.
	m_pMDIFrame->OnMDIDestroy(WM_MDIDESTROY, wParam, LPARAM(this), bHandled);
	delete this;
}