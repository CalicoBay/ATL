#include "pch.h"
#include "ATLXAMLChild.h"
#include "ATLXAML.h"
using namespace ATLControls;
using namespace winrt;

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
    m_sContent = _T("<Page xmlns=\"http://schemas.microsoft.com/winfx/2006/xaml/presentation\"\r\n\t\txmlns:x = \"http://schemas.microsoft.com/winfx/2006/xaml\">\r\n\t<Border BorderBrush=\"Red\"\r\n\t\tBorderThickness=\"12\"\r\n\t\tCornerRadius=\"24\"\r\n\t\tBackground=\"Yellow\"\r\n\t\tHorizontalAlignment=\"Center\"\r\n\t\tVerticalAlignment=\"Center\">\r\n\t<TextBlock Text=\"Hello XAML Cruncher!\"\r\n\t\tFontSize=\"48\"\r\n\t\tForeground=\"Blue\"\r\n\t\tMargin=\"24\" />\r\n\t</Border>\r\n</Page>");
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

LRESULT CATLXAMLChild::OnFileClose(WORD wHiParam, WORD wLoParam, HWND hwnd, BOOL& bHandled)
{
    bool bWM_CLOSE = false;
    if(0 == wLoParam)
    {//This is from a WM_CLOSE message instead of ID_FILE_CLOSE command
        bWM_CLOSE = true;
    }

    if(m_Edit.IsDirty())
    {
        int iResult;
        if(SUCCEEDED(::TaskDialog(__nullptr, __nullptr, __nullptr, m_Edit.m_sActivePath, L"Do you wish to save your changes?", TDCBF_YES_BUTTON | TDCBF_NO_BUTTON | TDCBF_CANCEL_BUTTON, TD_WARNING_ICON, &iResult)))
        {
            if(IDYES == iResult)
            {
                OnFileSave(0, 0, 0, bHandled);
            }
            else if(IDCANCEL == iResult)
            {
                bHandled = TRUE;
                if(!bWM_CLOSE)
                {
                    return 0;
                }
            }
        }
    }

    if(!bHandled || !bWM_CLOSE)
    {
        if(__nullptr != m_Edit.m_File.m_h)
        {
            m_Edit.m_File.Close();
        }

        if(!bWM_CLOSE)
        {//The window is staying open.
            m_Edit.SetSel(0, -1, TRUE);
            m_Edit.Clear();
            m_Edit.SetModify(FALSE);
        }

        m_sFilePath.Empty();
        WPARAM wParam = WM_SET_TITLE;
        SetWindowText(m_sFilePath);
        ::SendMessage(m_hwndFrame, WM_PARENTNOTIFY, wParam, (LPARAM)this);
    }

    if(!bWM_CLOSE)
    {
        bHandled = TRUE;
    }

    return 0;
}
LRESULT CATLXAMLChild::OnFileOpen(WORD, WORD, HWND, BOOL&)
{
    LRESULT lResult = 0;
    BOOL bHandled = TRUE;
    if(m_Edit.IsDirty() && m_sFilePath.IsEmpty())
    {
        lResult = OnFileSaveAs(0, 0, 0, bHandled);
    }
    com_ptr<IFileOpenDialog> com_ptrFileDialog;
    com_ptr<IShellItem> com_ptrShellItem;

    // CoCreate the dialog object.
    HRESULT hr = ::CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(com_ptrFileDialog.put()));
    if(SUCCEEDED(hr))
    {
        DWORD dwOptions;
        hr = com_ptrFileDialog->GetOptions(&dwOptions);
        if(SUCCEEDED(hr))
        {
            hr = com_ptrFileDialog->SetOptions(dwOptions | FOS_FORCEFILESYSTEM);
        }
        if(SUCCEEDED(hr))
        {
            hr = com_ptrFileDialog->Show(NULL);
        }

        if(SUCCEEDED(hr))
        {
            hr = com_ptrFileDialog->GetResult(com_ptrShellItem.put());
            if(SUCCEEDED(hr))
            {
                LPWSTR pwszPath;
                hr = com_ptrShellItem->GetDisplayName(SIGDN_FILESYSPATH, &pwszPath);
                if(SUCCEEDED(hr))
                {
                    //OpenDocumentFile(pwszPath);
                    CString csMessage;
                    //UINT cchTextLimit = m_Edit.GetLimitText();
                    UINT64 cbTextLimit = m_Edit.GetLimitText();// cchTextLimit;
                    cbTextLimit = cbTextLimit * sizeof(TCHAR);
                    csMessage.Format(_T("%s\r\nEdit Box Byte Limit: %I64u"), pwszPath, cbTextLimit);//0x%08I32X
                    int iResult;
                    hr = ::TaskDialog(__nullptr, __nullptr, _T("Opening file"), csMessage, _T("Is this what you want?"), TDCBF_YES_BUTTON | TDCBF_NO_BUTTON, TD_INFORMATION_ICON, &iResult);
                    if(SUCCEEDED(hr))
                    {
                        if(IDYES == iResult)
                        {
                            //This will only be done after succesfully opening a file
                            hr = m_Edit.OpenFile(pwszPath, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, OPEN_EXISTING);
                            if(SUCCEEDED(hr))
                            {
                                m_sFilePath = pwszPath;
                                SetWindowText(pwszPath);
                                WPARAM wParam = WM_SET_TITLE;
                                ::SendMessage(m_hwndFrame, WM_PARENTNOTIFY, wParam, (LPARAM)this);
                            }
                        }
                    }
                    ::CoTaskMemFree(pwszPath);
                }
            }
        }
    }
    m_Edit.SetFocus();
    return static_cast<LRESULT>(hr);
}
LRESULT CATLXAMLChild::OnFileSave(WORD, WORD, HWND, BOOL& bHandled)
{
    HRESULT hr = m_Edit.SaveFile();
    if(S_OK != hr)
    {
        int iResult;
        if(SUCCEEDED(::TaskDialog(__nullptr, __nullptr, __nullptr, __nullptr,
            L"The file could not be saved! Do you wish to save your changes using another path?",
            TDCBF_YES_BUTTON | TDCBF_CANCEL_BUTTON, TD_WARNING_ICON, &iResult)))
        {
            if(IDYES == iResult)
            {
                bHandled = FALSE;
                OnFileSaveAs(0, 0, 0, bHandled);
            }
        }
    }
    m_Edit.SetFocus();
    return 0;
}
LRESULT CATLXAMLChild::OnFileSaveAs(WORD, WORD, HWND, BOOL& bHandled)
{
    HRESULT hr;
    com_ptr<IFileSaveDialog> cpFileDialog;
    com_ptr<IShellItem> cpShellItem;
    PWSTR wszFilePath = __nullptr;
    hr = ::CoCreateInstance(CLSID_FileSaveDialog, __nullptr, CLSCTX_INPROC_SERVER, __uuidof(IFileSaveDialog), cpFileDialog.put_void());
    if(SUCCEEDED(hr))
    {
        hr = cpFileDialog->SetFileTypes(ARRAYSIZE(c_ATLEditFileTypes), c_ATLEditFileTypes);
        if(SUCCEEDED(hr))hr = cpFileDialog->SetFileTypeIndex(c_IndexAll);
        if(SUCCEEDED(hr))hr = cpFileDialog->SetDefaultExtension(L"xaml");
        if(SUCCEEDED(hr))hr = cpFileDialog->Show(__nullptr);
    }

    if(SUCCEEDED(hr))
    {
        hr = cpFileDialog->GetResult(cpShellItem.put());
        if(SUCCEEDED(hr))
        {
            hr = cpShellItem->GetDisplayName(SIGDN_FILESYSPATH, &wszFilePath);
            if(SUCCEEDED(hr))
            {
                hr = m_Edit.SaveFile(wszFilePath);
                if(SUCCEEDED(hr))
                {
                    if(!bHandled)
                    {
                        SetWindowText(wszFilePath);
                        m_sFilePath = wszFilePath;
                        WPARAM wParam = WM_SET_TITLE;
                        ::SendMessage(m_hwndFrame, WM_PARENTNOTIFY, wParam, (LPARAM)this);
                    }
                }

                if(S_OK != hr)
                {
                    CStringW sMessage;
                    sMessage.Format(L"The file\r\n%s\r\ncould not be saved!\r\nHRESULT: 0x%08X", wszFilePath, hr);
                    ::TaskDialog(__nullptr, __nullptr, __nullptr, __nullptr,
                        sMessage, TDCBF_YES_BUTTON, TD_WARNING_ICON, __nullptr);
                }

                ::CoTaskMemFree(wszFilePath);
            }
        }
    }
    m_Edit.SetFocus();
    return static_cast<LRESULT>(hr);
}
