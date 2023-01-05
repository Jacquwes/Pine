#pragma once
#include "User.g.h"

namespace winrt::Pine::implementation
{
    struct User : UserT<User>
    {
        User() = default;

        void AvatarUri(hstring const& value) { m_avatarUri = value; }
        hstring AvatarUri() { return m_avatarUri; }

        void BackgroundUri(hstring const& value) { m_backgroundUri = value; }
        hstring BackgroundUri() { return m_backgroundUri; }

        void City(hstring const& value) { m_city = value; }
        hstring City() { return m_city; }

        void Country(hstring const& value) { m_country = value; }
        hstring Country() { return m_country; }

        void Id(hstring const& value) { m_id = value; }
        hstring Id() { return m_id; }

        void Signature(hstring const& value) { m_signature = value; }
        hstring Signature() { return m_signature; }

        void Username(hstring const& value) { m_username = value; }
        hstring Username() { return m_username; }


    private:
        hstring m_avatarUri;
        hstring m_backgroundUri;
        hstring m_city;
        hstring m_country;
        hstring m_id;
        hstring m_signature;
        hstring m_username;
    };
}
namespace winrt::Pine::factory_implementation
{
    struct User : UserT<User, implementation::User>
    {
    };
}
