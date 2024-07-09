#include "pch.h"
#include "ATL-MDIChild.h"
#include "ATL-MDI.h"
#include <App.xaml.h>
#include <MainPage.h>
#include <Microsoft.UI.Dispatching.Interop.h> // For ContentPreTranslateMessage

namespace winrt
{
   using namespace winrt::Microsoft::UI;
   using namespace winrt::Microsoft::UI::Dispatching;
   using namespace winrt::Microsoft::UI::Xaml;
   using namespace winrt::Microsoft::UI::Xaml::Hosting;
   using namespace winrt::Microsoft::UI::Xaml::Markup;
}


static uint32_t begX = 0;
static short endX = 0;

// Extra state for our top-level window, we point to from GWLP_USERDATA.
struct WindowInfo
{
   winrt::DesktopWindowXamlSource DesktopWindowXamlSource{ nullptr };
   winrt::event_token TakeFocusRequestedToken{};
   HWND LastFocusedWindow{ NULL };
};

CATLMDIChild::CATLMDIChild(void) : 
   m_Static(_T("Static"), this),// , 1),// Now defaults to 0 re: //ALT_MSG_MAP(1) now commented out
   m_RectStatic{},
   m_hwndFrame(0),
   m_pMDIFrame(__nullptr),
   m_hSplitCursor(__nullptr)
{
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
         m_RectStatic.bottom = rcClient.bottom;//(rcClient.bottom - rcClient.top) / 2;

         HWND hwndStatic = m_Static.Create(m_hWnd, m_RectStatic);// , _T("XamlSource"), WS_CHILD | WS_BORDER | WS_VISIBLE);// | WS_VSCROLL | ES_MULTILINE | ES_READONLY
         if(__nullptr != hwndStatic)
         {
            WindowInfo* windowInfo = new WindowInfo();
            if(__nullptr != windowInfo)
            {
               ::SetWindowLongPtr(hwndStatic, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(windowInfo));
               // Create our DesktopWindowXamlSource and attach it to our hwnd.  This is our "island".
               windowInfo->DesktopWindowXamlSource = winrt::DesktopWindowXamlSource{};
               windowInfo->DesktopWindowXamlSource.Initialize(winrt::GetWindowIdFromWindow(hwndStatic));
               // Put a new instance of our Xaml "MainPage" into our island.  This is our UI content.
               windowInfo->DesktopWindowXamlSource.Content(winrt::make<winrt::ATLMDI::implementation::MainPage>());
            }
         }
      }
	}

	return 0;
}

LRESULT CATLMDIChild::OnClearText(WORD /*wHiParam*/, WORD /*wLoParam*/, HWND hwnd, BOOL& /*bHandled*/)
{
	m_sContent.Empty();
	return 0;
}

LRESULT CATLMDIChild::OnSetText(WORD /*wHiParam*/, WORD /*wLoParam*/, HWND hwnd, BOOL& /*bHandled*/)
{
	m_sContent = _T("TODO: We need to get some text from a TaskDialog for instance.");
	return 0;
}

LRESULT CATLMDIChild::OnSize(UINT, WPARAM, LPARAM, BOOL&)
{
   WindowInfo* windowInfo = reinterpret_cast<WindowInfo*>(::GetWindowLongPtr(m_Static, GWLP_USERDATA));
   RECT rcClient;
   GetClientRect(&rcClient);
   m_RectStatic = rcClient;
   if(__nullptr != windowInfo)
   {
      if(windowInfo->DesktopWindowXamlSource)
      {
         windowInfo->DesktopWindowXamlSource.SiteBridge().MoveAndResize({ rcClient.left, rcClient.top, rcClient.right, rcClient.bottom });
         ++begX;
         CString sText;
         sText.Format(_T("MoveAndResize called %i times."), begX);
         SetWindowText((LPCTSTR)sText);
      }
   }
   m_Static.SetWindowPos(HWND_TOP, &m_RectStatic, 0);// , SWP_NOCOPYBITS | SWP_SHOWWINDOW);//SWP_NOCOPYBITS | SWP_NOZORDER | 
   //RedrawWindow(0, 0, RDW_ERASE | RDW_FRAME | RDW_INVALIDATE);
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