#include "pch.h"
#include "SettingsTab.xaml.h"
#if __has_include("SettingsTab.g.cpp")
#include "SettingsTab.g.cpp"
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace winrt::Pine::implementation
{
    SettingsTab::SettingsTab()
    {
        InitializeComponent();
    }
}
