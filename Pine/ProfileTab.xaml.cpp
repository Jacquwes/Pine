#include "pch.h"
#include "ProfileTab.xaml.h"
#if __has_include("ProfileTab.g.cpp")
#include "ProfileTab.g.cpp"
#endif

#include "App.xaml.h"

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

	hstring ProfileTab::AvatarUri()
	{
		return App::Client().CurrentUser().AvatarUri().empty() ? L"ms-appx:///Assets/StoreLogo.png" : App::Client().CurrentUser().AvatarUri();
	}

	void ProfileTab::AvatarUri(hstring const& value)
	{
	}

	hstring ProfileTab::BackgroundUri()
	{
		return App::Client().CurrentUser().BackgroundUri().empty() ? L"ms-appx:///Assets/StoreLogo.png" : App::Client().CurrentUser().BackgroundUri();
	}

	void ProfileTab::BackgroundUri(hstring const& value)
	{
	}
	hstring ProfileTab::Id()
	{
		return App::Client().CurrentUser().Id();
	}
	void ProfileTab::Id(hstring const& value)
	{
	}
	hstring ProfileTab::Location()
	{
		return (App::Client().CurrentUser().City().empty() ? L"" : App::Client().CurrentUser().City() + L", ") + App::Client().CurrentUser().Country();
	}
	void ProfileTab::Location(hstring const& value)
	{
	}
	hstring ProfileTab::Signature()
	{
		return App::Client().CurrentUser().Signature();
	}
	void ProfileTab::Signature(hstring const& value)
	{
	}
	hstring ProfileTab::Username()
	{
		return App::Client().CurrentUser().Username();
	}
	void ProfileTab::Username(hstring const& value)
	{
	}
}
