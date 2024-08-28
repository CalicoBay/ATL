#include "pch.h"
#include "ATLXAMLChild.h"
#include "ATLXAML.h"

static short begX = 0;
static short endX = 0;

CATLXAMLChild::CATLXAMLChild(void) :
    m_Edit(_T("Edit"), this, 1),
    m_Static(_T("Static"), this, 2),
    m_hwndFrame(0),
    m_pMDIFrame(__nullptr),
    m_hSplitCursor(__nullptr)
{
    ::memset(&m_RectStatic, 0, sizeof(RECT));
    ::memset(&m_RectEdit, 0, sizeof(RECT));
    m_hSplitCursor = ::LoadCursor(__nullptr, MAKEINTRESOURCE(IDC_SIZEWE));
    m_sContent = _T("<Page xmlns=\"http://schemas.microsoft.com/winfx/2006/xaml/presentation\"\r\n\txmlns:x = \"http://schemas.microsoft.com/winfx/2006/xaml\">\r\n\t<Border BorderBrush=\"Red\"\r\n\t\tBorderThickness=\"12\"\r\n\t\tCornerRadius=\"24\"\r\n\t\tBackground=\"Yellow\"\r\n\t\tHorizontalAlignment=\"Center\"\r\n\t\tVerticalAlignment=\"Center\">\r\n\t<TextBlock Text=\"Hello XAML Cruncher!\"\r\n\t\tFontSize=\"48\"\r\n\t\tForeground=\"Blue\"\r\n\t\tMargin=\"24\" />\r\n\t</Border>\r\n</Page>");
    m_sStaticContent = _T("HWND hwndStatic = m_Static.Create(m_hWnd, m_RectStatic, m_sStaticContent, WS_CHILD | WS_BORDER | WS_VISIBLE);");
}


CATLXAMLChild::~CATLXAMLChild(void)
{
}

LRESULT CATLXAMLChild::OnCreate(UINT /*nMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled)
{
    bHandled = FALSE;
    CREATESTRUCT* pCreate = (CREATESTRUCT*)lParam;
    if(__nullptr != pCreate)
    {
        MDICREATESTRUCT* pMDICreate = (MDICREATESTRUCT*)pCreate->lpCreateParams;
        if(__nullptr != pMDICreate)
        {
            m_pMDIFrame = (CATLXAMLFrame*)pMDICreate->lParam;
            if(__nullptr == m_pMDIFrame)
            {
                return -1;
            }
            m_hwndFrame = m_pMDIFrame->m_hWnd;
            m_sOriginalTitle = pMDICreate->szTitle;
            RECT rcClient;
            GetClientRect(&rcClient);
            m_RectEdit.left = rcClient.left;
            m_RectEdit.top = rcClient.top;
            m_RectEdit.right = 250;
            m_RectEdit.bottom = rcClient.bottom;

            m_RectStatic.left = rcClient.left + 255;
            m_RectStatic.top = rcClient.top;
            m_RectStatic.right = rcClient.right;
            m_RectStatic.bottom = rcClient.bottom;
            HWND hwndStatic = m_Static.Create(m_hWnd, m_RectStatic, m_sStaticContent, WS_CHILD | WS_BORDER | WS_VISIBLE);// | WS_VSCROLL | ES_MULTILINE | ES_READONLY);
            HWND hwndEdit = m_Edit.Create(this, 1, m_hWnd, m_RectEdit);//(m_hWnd, m_RectEdit, m_sContent, WS_CHILD | WS_BORDER | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL | ES_MULTILINE);
            if(0 != hwndEdit)
            {
               m_Edit.SetSel(0, 1);
               m_Edit.ReplaceSel((LPCTSTR)m_sContent);
            }
        }
    }

    return 0;
}

LRESULT CATLXAMLChild::OnClearText(WORD /*wHiParam*/, WORD /*wLoParam*/, HWND hwnd, BOOL& /*bHandled*/)
{
    m_sContent.Empty();
    //m_Edit.SendMessage(EM_SETSEL, 0, -1);
    //m_Edit.SendMessage(EM_REPLACESEL, FALSE, (LPARAM)m_sContent.GetString());
    m_Edit.SetSel(0, -1);
    m_Edit.ReplaceSel((LPCTSTR)m_sContent);
    //Invalidate();
    return 0;
}

LRESULT CATLXAMLChild::OnSetText(WORD /*wHiParam*/, WORD /*wLoParam*/, HWND hwnd, BOOL& /*bHandled*/)
{
    m_sContent = _T("TODO: We need to get some text from a TaskDialog for instance.");
    m_Edit.SetSel(0, -1);
    m_Edit.ReplaceSel((LPCTSTR)m_sContent);
    //m_Edit.SendMessage(EM_SETSEL, 0, -1);
    //m_Edit.SendMessage(EM_REPLACESEL, FALSE, (LPARAM)m_sContent.GetString());
    //Invalidate();
    return 0;
}

LRESULT CATLXAMLChild::OnPaint(UINT /*nMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
    PAINTSTRUCT ps;
    HDC hdc = GetDC();
    RECT rectToPaint;
    HRGN rgnToPaint;
    rectToPaint.bottom = m_RectStatic.bottom;
    rectToPaint.left = m_RectStatic.right;
    rectToPaint.right = m_RectEdit.left;
    rectToPaint.top = m_RectEdit.top;
    rgnToPaint = ::CreateRectRgnIndirect(&rectToPaint);
    BeginPaint(&ps);
    ::FillRgn(hdc, rgnToPaint, GetSysColorBrush(COLOR_ACTIVECAPTION));
    EndPaint(&ps);

    return 0;
}

LRESULT CATLXAMLChild::OnSize(UINT, WPARAM, LPARAM, BOOL&)
{
    RECT rcClient;
    GetClientRect(&rcClient);
    m_RectEdit.top = m_RectStatic.top = rcClient.top;
    m_RectEdit.bottom = m_RectStatic.bottom = rcClient.bottom;
    if(rcClient.right > (m_RectEdit.right + 150))
    {
        m_RectStatic.right = rcClient.right;
    }
    else
    {
        m_RectStatic.right = m_RectEdit.right + 150;
    }
    if(m_Static.IsWindow())
    {
        m_Static.SetWindowPos(HWND_TOP, &m_RectStatic, 0);
    }
    if(m_Edit.IsWindow())
    {
        m_Edit.SetWindowPos(HWND_TOP, &m_RectEdit, 0);
    }
    return 0;
}

LRESULT CATLXAMLChild::OnMouseMove(UINT /*nMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
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

LRESULT CATLXAMLChild::OnLButtonDown(UINT /*nMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    RECT rcClient;
    GetWindowRect(&rcClient);
    rcClient.left += 100;
    rcClient.right -= 150;
    ::ClipCursor(&rcClient);
    begX = GET_X_LPARAM(lParam);
    ::SetCursor(m_hSplitCursor);
    ::SetCapture(m_hWnd);
    return 0;
}

LRESULT CATLXAMLChild::OnLButtonUp(UINT /*nMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    endX = GET_X_LPARAM(lParam);
    short deltaX = endX - begX;
    m_RectEdit.right += deltaX;
    m_RectStatic.left += deltaX;
    m_Static.SetWindowPos(HWND_TOP, &m_RectStatic, 0);
    m_Edit.SetWindowPos(HWND_TOP, &m_RectEdit, 0);
    ::ClipCursor(NULL);
    ::ReleaseCapture();
    return 0;
}

VOID CATLXAMLChild::OnFinalMessage(HWND /*hwnd*/)
{
    WPARAM wParam = (WPARAM)((LPCTSTR)m_sOriginalTitle);
    BOOL bHandled = FALSE;
    //::SendMessage(m_hwndFrame, WM_MDIDESTROY, wParam, (LPARAM)this);
    //Now that we have a pFrame no need to send, just call.
    m_pMDIFrame->OnMDIDestroy(WM_MDIDESTROY, wParam, LPARAM(this), bHandled);
    delete this;
}