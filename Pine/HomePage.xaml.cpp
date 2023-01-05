#include "pch.h"
#include "HomePage.xaml.h"
#if __has_include("HomePage.g.cpp")
#include "HomePage.g.cpp"
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace winrt::Pine::implementation
{
	HomePage::HomePage()
	{
		InitializeComponent();
		View().Navigate(Windows::UI::Xaml::Interop::TypeName{ L"Pine.ProfileTab" });
	}

	void HomePage::NavView_SelectionChanged(Controls::NavigationView const&, Controls::NavigationViewSelectionChangedEventArgs const& args)
	{
		hstring clickedName = 
			args.IsSettingsSelected() ?
			L"Pine.SettingsTab" :
			L"Pine." + unbox_value<hstring>(args.SelectedItem().as<Controls::NavigationViewItem>().Tag());
		View().Navigate(Windows::UI::Xaml::Interop::TypeName{ clickedName });
	}
}