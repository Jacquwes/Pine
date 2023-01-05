#pragma once

#include "App.xaml.g.h"

namespace winrt::Pine::implementation
{
	struct App : AppT<App>
	{
		App();

		void OnLaunched(Microsoft::UI::Xaml::LaunchActivatedEventArgs const&);
		static Pine::PineClient Client();

	private:
		winrt::Microsoft::UI::Xaml::Window window{ nullptr };
		static Pine::PineClient m_client;
	};
}