// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
#include "pch.h"

#include "MainPage.h"

namespace winrt
{
	using namespace winrt::Microsoft::UI;
	using namespace winrt::Microsoft::UI::Dispatching;
	using namespace winrt::Microsoft::UI::Xaml;
   using namespace winrt::Microsoft::UI::Xaml::Controls;
   using namespace winrt::Microsoft::UI::Xaml::Hosting;
	using namespace winrt::Microsoft::UI::Xaml::Markup;
   using namespace winrt::Windows::Foundation;
}

namespace winrt::ATLMDI::implementation
{
    int32_t MainPage::MyProperty()
    {
        throw hresult_not_implemented();
    }

    void MainPage::MyProperty(int32_t /* value */)
    {
        throw hresult_not_implemented();
    }

    Uri MainPage::Source()
    {
       return OurWeb().Source();
    }

    void MainPage::Source(Uri const& value)
    {
       OurWeb().Source(value);
    }

    void MainPage::ClickHandler(IInspectable const&, RoutedEventArgs const&)
    {
      try
      {
         Uri targetUri(addressBar().Text());
         OurWeb().Source(targetUri);
      }
      catch(const winrt::hresult_error& exception)
      {
         CString sTryAgain;
         sTryAgain.Format(_T("%s Try again"), exception.message().c_str());
         addressBar().Text((LPCTSTR)sTryAgain);
      }
    }

    void MainPage::NavStarting(IInspectable const& sender, Microsoft::Web::WebView2::Core::CoreWebView2NavigationStartingEventArgs const& args)
    {
       if(0 == m_pendingNavID)
       {
          m_pendingUri = args.Uri();
          m_pendingNavID = args.NavigationId();
       }
       else
       {
          bool bSomethingIsPending = true;
       }
    }

    void MainPage::NavCompleted(Windows::Foundation::IInspectable const& sender, Microsoft::Web::WebView2::Core::CoreWebView2NavigationCompletedEventArgs const& args)
    {
       if(args.IsSuccess())
       {
          if(args.NavigationId() == m_pendingNavID)
          {
             addressBar().Text(m_pendingUri.c_str());
             m_pendingNavID = 0;
          }
       }
       else
       {
          bool bNavFailed = true;
          m_pendingNavID = 0;
          int32_t error = args.HttpStatusCode();
          m_pendingUri.clear();
       }
    }
}
