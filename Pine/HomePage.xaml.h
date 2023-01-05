#pragma once

#include "HomePage.g.h"

namespace winrt::Pine::implementation
{
    struct HomePage : HomePageT<HomePage>
    {
        HomePage();
        void NavView_SelectionChanged(winrt::Microsoft::UI::Xaml::Controls::NavigationView const& sender, winrt::Microsoft::UI::Xaml::Controls::NavigationViewSelectionChangedEventArgs const& args);
    };
}

namespace winrt::Pine::factory_implementation
{
    struct HomePage : HomePageT<HomePage, implementation::HomePage>
    {
    };
}
