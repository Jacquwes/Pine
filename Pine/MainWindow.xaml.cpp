#include "pch.h"
#include "MainWindow.xaml.h"
#if __has_include("MainWindow.g.cpp")
#include "MainWindow.g.cpp"
#endif

#include "App.xaml.h"
#include "ConnectionPage.xaml.h"
#include "PineClient.h"

using namespace winrt;
using namespace Microsoft::UI::Xaml;

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace winrt::Pine::implementation
{
	MainWindow::MainWindow()
	{
		InitializeComponent();

		ExtendsContentIntoTitleBar(true);
		SetTitleBar(TitleBar());
		
#pragma region Backdrop
		if (Microsoft::UI::Composition::SystemBackdrops::MicaController::IsSupported())
		{
			if (!WS::DispatcherQueue::GetForCurrentThread() && !m_dispatcherQueueController)
			{
				::ABI::Windows::System::IDispatcherQueueController* ptr{ nullptr };
				check_hresult(CreateDispatcherQueueController({ sizeof(DispatcherQueueOptions), DQTYPE_THREAD_CURRENT, DQTAT_COM_NONE }, &ptr));
				m_dispatcherQueueController = { ptr, take_ownership_from_abi };
			}

			m_backdropController = Microsoft::UI::Composition::SystemBackdrops::MicaController();
		}

		m_backdropConfiguration = MUCSB::SystemBackdropConfiguration();
		
		m_activatedRevoker = Activated(auto_revoke,
			[&](auto&&, MUX::WindowActivatedEventArgs const& args)
			{
				m_backdropConfiguration.IsInputActive(args.WindowActivationState() != MUX::WindowActivationState::Deactivated);
			});
		
		m_backdropConfiguration.IsInputActive(true);

		m_rootElement = Content().try_as<MUX::FrameworkElement>();
		if (!m_rootElement)
		{
			m_actualThemeChangedRevoker = m_rootElement.ActualThemeChanged(auto_revoke,
				[&](auto&&, auto&&)
				{
					m_backdropConfiguration.Theme(MUCSB::SystemBackdropTheme::Dark);
				});
			
			m_backdropConfiguration.Theme(MUCSB::SystemBackdropTheme::Dark);
		}

		m_backdropController = MUCSB::MicaController();
		m_backdropController.SetSystemBackdropConfiguration(m_backdropConfiguration);
		m_backdropController.AddSystemBackdropTarget(this->try_as<MUC::ICompositionSupportsSystemBackdrop>());

		m_closedRevoker = Closed(auto_revoke,
			[&](auto&&, auto&&)
			{
				if (!m_backdropController)
				{
					m_backdropController.Close();
					m_backdropController = nullptr;
				}

				if (!m_dispatcherQueueController)
				{
					m_dispatcherQueueController.ShutdownQueueAsync();
					m_dispatcherQueueController = nullptr;
				}
			});
#pragma endregion
		
		if (false)
		{
			// display menu
		}
		else
		{
			MainFrame().Navigate(Windows::UI::Xaml::Interop::TypeName{ L"Pine.ConnectionPage" });
		}
	}

	MUX::Controls::Frame MainWindow::GetMainFrame()
	{
		return MainFrame();
	}
}
