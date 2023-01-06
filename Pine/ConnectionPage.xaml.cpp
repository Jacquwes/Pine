#include "pch.h"
#include "ConnectionPage.xaml.h"
#if __has_include("ConnectionPage.g.cpp")
#include "ConnectionPage.g.cpp"
#endif

#include "App.xaml.h"
#include "Utils.h"

using namespace winrt;
using namespace Microsoft::UI::Xaml;
using namespace Windows::Security::Cryptography;
using namespace Windows::Security::Cryptography::Core;

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace winrt::Pine::implementation
{
	ConnectionPage::ConnectionPage()
	{
		InitializeComponent();
	}

	void ConnectionPage::OnNavigatedTo(Navigation::NavigationEventArgs const& e)
	{
	}

	Windows::Foundation::IAsyncAction ConnectionPage::TryConnect()
	{
		if (Id().Text().empty() || Password().Password().empty())
		{
			ErrorMessage().Text(L"Veuillez renseigner tous les champs.");
			co_return;
		}
		else
			ErrorMessage().Text(L"");
		
		hstring algo = HashAlgorithmNames::Sha512();
		HashAlgorithmProvider hasher = HashAlgorithmProvider::OpenAlgorithm(algo);
		Windows::Storage::Streams::IBuffer hashed = hasher.HashData(CryptographicBuffer::ConvertStringToBinary(Password().Password(), BinaryStringEncoding::Utf8));
		hstring hashedPassword = CryptographicBuffer::EncodeToHexString(hashed);

		try
		{
			co_await App::Client().LoginAsync(Id().Text(), hashedPassword);
		}
		catch (hresult_error const& e)
		{
			if ((e.code() & 0x80070000) == 0x80070000)
				ErrorMessage().Text(L"Erreur de connexion au serveur.");
			else
				ErrorMessage().Text(L"Identifiant ou mot de passe incorrect.");
			
			co_return;
		}

		Parent().as<Controls::Frame>().Navigate(Windows::UI::Xaml::Interop::TypeName{ L"Pine.HomePage" });
	}

	Windows::Foundation::IAsyncAction ConnectionPage::TrySignUp()
	{
		if (Id().Text().empty() || Password().Password().empty())
		{
			ErrorMessage().Text(L"Veuillez renseigner tous les champs.");
			co_return;
		}
		else
			ErrorMessage().Text(L"");

		hstring algo = HashAlgorithmNames::Sha512();
		HashAlgorithmProvider hasher = HashAlgorithmProvider::OpenAlgorithm(algo);
		Windows::Storage::Streams::IBuffer hashed = hasher.HashData(CryptographicBuffer::ConvertStringToBinary(Password().Password(), BinaryStringEncoding::Utf8));
		hstring hashedPassword = CryptographicBuffer::EncodeToHexString(hashed);

		try
		{
			co_await App::Client().SignUpAsync(Id().Text(), hashedPassword);
		}
		catch (hresult_error const& e)
		{
			if ((e.code() & 0x80070000) == 0x80070000)
				ErrorMessage().Text(L"Erreur de connexion au serveur.");
			else
				ErrorMessage().Text(L"Identifiant ou mot de passe incorrect.");

			co_return;
		}

		Parent().as<Controls::Frame>().Navigate(Windows::UI::Xaml::Interop::TypeName{ L"Pine.HomePage" });
	}
	
	void ConnectionPage::PasswordBox_KeyDown(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::Input::KeyRoutedEventArgs const& e)
	{
		if (e.Key() == Windows::System::VirtualKey::Enter)
			TryConnect();
	}

	void ConnectionPage::LoginButton_Click(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::RoutedEventArgs const&)
	{
		TryConnect();
	}

	void ConnectionPage::SignUpButton_Click(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::RoutedEventArgs const&)
	{
		TrySignUp();
	}

	void ConnectionPage::Id_KeyDown(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::Input::KeyRoutedEventArgs const& e)
	{
		if (e.Key() == Windows::System::VirtualKey::Enter)
		{
			if (Password().Password().empty())
				Password().Focus(Microsoft::UI::Xaml::FocusState::Keyboard);
			else if (!Id().Text().empty())
				TryConnect();
				
		}
		
	}
}
