#pragma once

#include "ContactsTab.g.h"

namespace winrt::Pine::implementation
{
    struct ContactsTab : ContactsTabT<ContactsTab>
    {
        ContactsTab();
    };
}

namespace winrt::Pine::factory_implementation
{
    struct ContactsTab : ContactsTabT<ContactsTab, implementation::ContactsTab>
    {
    };
}
