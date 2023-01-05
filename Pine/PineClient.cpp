#include "pch.h"
#include "PineClient.h"
#include "PineClient.g.cpp"

namespace winrt::Pine::implementation
{
	Windows::Foundation::IAsyncAction PineClient::LoginAsync(hstring id, hstring password)
	{
		Windows::Web::Http::HttpClient client;
		Windows::Foundation::Uri uri{ L"http://127.0.0.1/api/v1/users/login" };
		Windows::Web::Http::HttpResponseMessage res;
		std::wstring body;

		try
		{
			res = co_await client.PostAsync(uri,
											Windows::Web::Http::HttpStringContent(L"{\"id\":\"" + id + L"\",\"password\":\"" + password + L"\"}",
											Windows::Storage::Streams::UnicodeEncoding::Utf8,
											L"application/json"));
			res.EnsureSuccessStatusCode();
			hstring token = Windows::Data::Json::JsonObject::Parse(co_await res.Content().ReadAsStringAsync()).GetNamedString(L"token");
			m_token = token;
		}
		catch (hresult_error const& e)
		{
			throw e;
		}
	}
}
