#include "pch.h"
#include "ATLMDI.h"
#include <App.xaml.h>
#include <MainPage.h>
#include "ATLMDIChild.h"
#include <Microsoft.UI.Dispatching.Interop.h> // For ContentPreTranslateMessage

namespace winrt
{
   using namespace winrt::Microsoft::UI;
   using namespace winrt::Microsoft::UI::Dispatching;
   using namespace winrt::Microsoft::UI::Xaml;
   using namespace winrt::Microsoft::UI::Xaml::Hosting;
   using namespace winrt::Microsoft::UI::Xaml::Markup;
   using namespace winrt::Windows::Foundation;
}


//static uint32_t begX = 0;
//static short endX = 0;

CATLMDIChild::CATLMDIChild(void) : 
   m_Static(_T("Static"), this),// , 1),// Now defaults to 0 re: //ALT_MSG_MAP(1) now commented out
   m_RectStatic{},
   m_hwndFrame(__nullptr),
   m_hwndXamlIsland(__nullptr),
   m_pMDIFrame(__nullptr),
   m_pWindowInfo(__nullptr),
   m_hSplitCursor(__nullptr),
   m_bActivated(false)
{
   m_hSplitCursor = ::LoadCursor(__nullptr, MAKEINTRESOURCE(IDC_SIZENS));
   m_sContent = _T("https://learn.microsoft.com/en-gb/windows/apps/desktop/");
   m_sStaticContent = _T("https://learn.microsoft.com/en-us/uwp/midl-3/");
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

         HWND hwndFrame = m_hwndFrame;
         HWND hwndStatic = m_Static.Create(m_hWnd, m_RectStatic);// , _T("XamlSource"), WS_CHILD | WS_BORDER | WS_VISIBLE);// | WS_VSCROLL | ES_MULTILINE | ES_READONLY
         if(__nullptr != hwndStatic)
         {
            //BOOL bModifiedStyle = m_Static.ModifyStyle(0, WS_TABSTOP);
            m_pWindowInfo = new WindowInfo();
            if(__nullptr != m_pWindowInfo)
            {
               // Create our DesktopWindowXamlSource and save it to our WindowInfo.  This is our "island".
               m_pWindowInfo->DesktopWindowXamlSource = winrt::DesktopWindowXamlSource{};
               m_pWindowInfo->DesktopWindowXamlSource.Initialize(winrt::GetWindowIdFromWindow(hwndStatic));

               // Enable the DesktopWindowXamlSource to be a tab stop.
               m_hwndXamlIsland = winrt::GetWindowFromWindowId(m_pWindowInfo->DesktopWindowXamlSource.SiteBridge().WindowId());
               ::SetWindowLong(m_hwndXamlIsland, GWL_STYLE, WS_TABSTOP | WS_CHILD | WS_VISIBLE);

               // Put a new instance of our Xaml "MainPage" into our island.  This is our UI content.
               m_MainPage = winrt::make<winrt::ATLMDI::implementation::MainPage>();
               m_pWindowInfo->DesktopWindowXamlSource.Content(m_MainPage);

               try
               {// Try what's in m_sStaticContent, could end up as an index to a list??
                  winrt::Uri uri((LPCTSTR)m_sStaticContent);
                  m_MainPage.Source(uri);
                  //SetWindowText(uri.ToString().c_str());
               }
               catch(...)
               {// Fall back on the Xaml - Source="https://aka.ms/windev"
                  winrt::Uri uri = m_MainPage.Source();
                  //SetWindowText(uri.ToString().c_str());
               }

               //HWND hwndPrev = ::SetFocus(m_hwndXamlIsland);
               //bool bFocusSet = (0 != hwndPrev);//m_pWindowInfo->DesktopWindowXamlSource.Content().Focus(winrt::Microsoft::UI::Xaml::FocusState::Programmatic);//::SetFocus(m_hwndXamlIsland);
               //if(!bFocusSet)
               //{// Only the first time, doesn't make any difference anyhow, probably shows in focus or OnSize does it
               //   MessageBox(_T("Focus not set on m_hwndXamlIsland."));
               //}
               //else
               //{
               //   bFocusSet = (hwndPrev == hwndStatic);
               //   bFocusSet = (hwndPrev == m_hWnd);
               //   bFocusSet = (hwndPrev == m_hwndFrame);
               //}

               // Subscribe to the TakeFocusRequested event, which will be raised when Xaml wants to move keyboard focus back to our window.
               //m_pWindowInfo->TakeFocusRequestedToken = m_pWindowInfo->DesktopWindowXamlSource.TakeFocusRequested(
               //   [this](winrt::DesktopWindowXamlSource const& /*sender*/, winrt::DesktopWindowXamlSourceTakeFocusRequestedEventArgs const& args) {
               //      if(args.Request().Reason() == winrt::XamlSourceFocusNavigationReason::First)
               //      {
               //         // The reason "First" means the user is tabbing forward, so put the focus on the button in the tab order
               //         // after the DesktopWindowXamlSource.
               //         m_Static.SetFocus(); //::SetFocus(m_hwndXamlIsland);
               //      }
               //      else if(args.Request().Reason() == winrt::XamlSourceFocusNavigationReason::Last)
               //      {
               //         // The reason "Last" means the user is tabbing backward (shift-tab, so put the focus on button prior to
               //         // the DesktopWindowXamlSource.
               //         m_Static.SetFocus(); //::SetFocus(m_hwndXamlIsland);
               //      }
               //   });
            }
         }
      }
	}

	return 0;
}

LRESULT CATLMDIChild::OnSetFocus(UINT /*nMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
   ::SetFocus(m_hwndXamlIsland);
   return 0;
}

//LRESULT CATLMDIChild::OnMDIActivate(UINT /*nMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
//{
//   if(HWND(lParam) == m_hWnd)
//   {
//      m_bActivated = true;
//      m_pWindowInfo->LastFocusedWindow = ::GetFocus();
//   }
//   else if(HWND(wParam) == m_hWnd)
//   {
//      m_bActivated = false;
//      ::SetFocus(m_pWindowInfo->LastFocusedWindow);
//   }
//   return 0;
//}

LRESULT CATLMDIChild::OnNcDestroy(UINT /*nMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
   if(__nullptr != m_pWindowInfo)
   {
      if((m_pWindowInfo->DesktopWindowXamlSource) && (0 != m_pWindowInfo->TakeFocusRequestedToken.value))
      {
         m_pWindowInfo->DesktopWindowXamlSource.TakeFocusRequested(m_pWindowInfo->TakeFocusRequestedToken);
         m_pWindowInfo->TakeFocusRequestedToken = {};
      }
      delete m_pWindowInfo;
      m_pWindowInfo = __nullptr;
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
   RECT rcClient;
   GetClientRect(&rcClient);
   m_RectStatic = rcClient;
   if(__nullptr != m_pWindowInfo)
   {
      if(m_pWindowInfo->DesktopWindowXamlSource)
      {
         m_pWindowInfo->DesktopWindowXamlSource.SiteBridge().MoveAndResize({ rcClient.left, rcClient.top, rcClient.right, rcClient.bottom });
         //++begX;
         //CString sText;
         //sText.Format(_T("MoveAndResize called %i times."), begX);
         //SetWindowText((LPCTSTR)sText);
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