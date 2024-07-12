// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
#pragma once

#include "MainPage.g.h"

namespace winrt::ATLMDI::implementation
{
    struct MainPage : MainPageT<MainPage>
    {
        MainPage() 
        {
            // Xaml objects should not call InitializeComponent during construction.
            // See https://github.com/microsoft/cppwinrt/tree/master/nuget#initializecomponent
        }

        int32_t MyProperty();
        void MyProperty(int32_t value);
        winrt::Windows::Foundation::Uri GetSource();

        void ClickHandler(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::RoutedEventArgs const& args);
    };
}

namespace winrt::ATLMDI::factory_implementation
{
    struct MainPage : MainPageT<MainPage, implementation::MainPage>
    {
    };
}
