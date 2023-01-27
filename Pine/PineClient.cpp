#include "pch.h"
#include "PineClient.h"
#include "PineClient.g.cpp"

using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::Data::Json;
using namespace Windows::Web;

namespace winrt::Pine::implementation
{
	Pine::User PineClient::CurrentUser()
	{
		return m_currentUser;
	}

	IAsyncAction PineClient::LoginAsync(hstring id, hstring password)
	{

	}

	IAsyncAction PineClient::SignUpAsync(hstring id, hstring password)
	{

	}
	
	IAsyncAction PineClient::FetchCurrentUserAsync()
	{

	}
}
