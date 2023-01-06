#pragma once
#include "PineClient.g.h"

namespace winrt::Pine::implementation
{
	struct PineClient : PineClientT<PineClient>
	{
		PineClient() = default;

		Windows::Foundation::Collections::IObservableVector<Pine::User> Contacts() { return m_contacts; }

		Windows::Foundation::IAsyncAction LoginAsync(hstring id, hstring password);
		Windows::Foundation::IAsyncAction SignUpAsync(hstring id, hstring password);

	private:
		Windows::Foundation::Collections::IObservableVector<Pine::User> m_contacts;
		hstring m_token;
	};
}
namespace winrt::Pine::factory_implementation
{
	struct PineClient : PineClientT<PineClient, implementation::PineClient>
	{
	};
}
