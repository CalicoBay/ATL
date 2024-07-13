// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
#pragma once

#include "MainPage.g.h"

namespace winrt::ATLMDI::implementation
{
   struct MainPage : MainPageT<MainPage>
   {
      winrt::hstring m_pendingUri;
      uint64_t m_pendingNavID;
      MainPage() : m_pendingNavID(0)
      {
         // Xaml objects should not call InitializeComponent during construction.
         // See https://github.com/microsoft/cppwinrt/tree/master/nuget#initializecomponent
      }

      int32_t MyProperty();
      void MyProperty(int32_t value);
      Windows::Foundation::Uri Source();
      void Source(Windows::Foundation::Uri const& value);

      void ClickHandler(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::RoutedEventArgs const& args);
      void NavStarting(Windows::Foundation::IInspectable const& sender, Microsoft::Web::WebView2::Core::CoreWebView2NavigationStartingEventArgs const& args);
      void NavCompleted(Windows::Foundation::IInspectable const& sender, Microsoft::Web::WebView2::Core::CoreWebView2NavigationCompletedEventArgs const& args);
   };
}

namespace winrt::ATLMDI::factory_implementation
{
   struct MainPage : MainPageT<MainPage, implementation::MainPage>
   {
   };
}
