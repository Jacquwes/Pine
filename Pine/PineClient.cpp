#include "pch.h"
#include "PineClient.h"
#include "PineClient.g.cpp"

namespace winrt::Pine::implementation
{
	Pine::User PineClient::CurrentUser()
	{
		return m_currentUser;
	}

	Windows::Foundation::IAsyncAction PineClient::LoginAsync(hstring id, hstring password)
	{
		Windows::Foundation::Uri uri{ L"http://127.0.0.1/api/v1/users/login" };
		Windows::Web::Http::HttpResponseMessage res;
		std::wstring body;

		try
		{
			res = co_await m_client.PostAsync(uri,
											Windows::Web::Http::HttpStringContent(L"{\"id\":\"" + id + L"\",\"password\":\"" + password + L"\"}",
											Windows::Storage::Streams::UnicodeEncoding::Utf8,
											L"application/json"));
			res.EnsureSuccessStatusCode();
			hstring token = Windows::Data::Json::JsonObject::Parse(co_await res.Content().ReadAsStringAsync()).GetNamedString(L"token");
			m_token = token;
			m_client.DefaultRequestHeaders().Authorization(Windows::Web::Http::Headers::HttpCredentialsHeaderValue(L"User", token));
			co_await FetchCurrentUserAsync();
		}
		catch (hresult_error const& e)
		{
			throw e;
		}
	}

	Windows::Foundation::IAsyncAction PineClient::SignUpAsync(hstring id, hstring password)
	{
		Windows::Foundation::Uri uri{ L"http://127.0.0.1/api/v1/users/signup" };
		Windows::Web::Http::HttpResponseMessage res;
		std::wstring body;

		try
		{
			res = co_await m_client.PostAsync(uri,
											Windows::Web::Http::HttpStringContent(L"{\"id\":\"" + id + L"\",\"password\":\"" + password + L"\"}",
											Windows::Storage::Streams::UnicodeEncoding::Utf8,
											L"application/json"));
			res.EnsureSuccessStatusCode();
			hstring token = Windows::Data::Json::JsonObject::Parse(co_await res.Content().ReadAsStringAsync()).GetNamedString(L"token");
			m_token = token;
			m_client.DefaultRequestHeaders().Authorization(Windows::Web::Http::Headers::HttpCredentialsHeaderValue(token));
			co_await FetchCurrentUserAsync();
		}
		catch (hresult_error const& e)
		{
			throw e;
		}
	}
	
	Windows::Foundation::IAsyncAction PineClient::FetchCurrentUserAsync()
	{
		Windows::Foundation::Uri uri{ L"http://127.0.0.1/api/v1/users/me" };
		Windows::Web::Http::HttpResponseMessage res;
		std::wstring body;

		hstring token = m_client.DefaultRequestHeaders().Authorization().Token();
		hstring str = m_client.DefaultRequestHeaders().Authorization().ToString();

		try
		{
			res = co_await m_client.GetAsync(uri);
			res.EnsureSuccessStatusCode();
			auto json = Windows::Data::Json::JsonObject::Parse(co_await res.Content().ReadAsStringAsync());
			
			m_currentUser = Pine::User();
			m_currentUser.AvatarUri(json.GetNamedString(L"avataruri"));
			m_currentUser.BackgroundUri(json.GetNamedString(L"backgrounduri"));
			m_currentUser.City(json.GetNamedString(L"city"));
			m_currentUser.Country(json.GetNamedString(L"country"));
			m_currentUser.Id(json.GetNamedString(L"id"));
			m_currentUser.Signature(json.GetNamedString(L"signature"));
			m_currentUser.Username(json.GetNamedString(L"username"));
		}
		catch (hresult_error const& e)
		{
			throw e;
		}
	}
}
