#pragma once

#include "User.h"

#include "ChatTab.g.h"

namespace winrt::Pine::implementation
{
    struct ChatTab : ChatTabT<ChatTab>
    {
        ChatTab();
    };
}

namespace winrt::Pine::factory_implementation
{
    struct ChatTab : ChatTabT<ChatTab, implementation::ChatTab>
    {
    };
}
