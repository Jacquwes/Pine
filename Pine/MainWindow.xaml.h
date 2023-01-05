#pragma once

#include "MainWindow.g.h"

#include "PineClient.h"

namespace winrt
{
	namespace MUC = Microsoft::UI::Composition;
	namespace MUCSB = MUC::SystemBackdrops;
	namespace MUX = Microsoft::UI::Xaml;
	namespace WS = Windows::System;
}

namespace winrt::Pine::implementation
{
	struct MainWindow : MainWindowT<MainWindow>
	{
		MainWindow();

		MUX::Controls::Frame GetMainFrame();

	private:
		MUCSB::MicaController m_backdropController{ nullptr };
		MUCSB::SystemBackdropConfiguration m_backdropConfiguration{ nullptr };
		MUX::Window::Activated_revoker m_activatedRevoker{};
		MUX::Window::Closed_revoker m_closedRevoker{};
		MUX::FrameworkElement::ActualThemeChanged_revoker m_actualThemeChangedRevoker{};
		MUX::FrameworkElement m_rootElement{ nullptr };
		WS::DispatcherQueueController m_dispatcherQueueController{ nullptr };
	};
}

namespace winrt::Pine::factory_implementation
{
	struct MainWindow : MainWindowT<MainWindow, implementation::MainWindow>
	{
	};
}
