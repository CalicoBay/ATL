// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
#include "pch.h"

#include "MainPage.h"

namespace winrt
{
	using namespace winrt::Microsoft::UI;
	using namespace winrt::Microsoft::UI::Dispatching;
	using namespace winrt::Microsoft::UI::Xaml;
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
}
