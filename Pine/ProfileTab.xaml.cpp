#include "pch.h"
#include "ProfileTab.xaml.h"
#if __has_include("ProfileTab.g.cpp")
#include "ProfileTab.g.cpp"
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace winrt::Pine::implementation
{
	ProfileTab::ProfileTab()
	{
		InitializeComponent();
		NavigationCacheMode(Navigation::NavigationCacheMode::Enabled);

	}

	hstring ProfileTab::BackgroundUri()
	{
		return hstring();
	}

	void ProfileTab::BackgroundUri(hstring const& value)
	{
	}
}
