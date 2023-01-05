#pragma once

#include "ForumTab.g.h"

namespace winrt::Pine::implementation
{
    struct ForumTab : ForumTabT<ForumTab>
    {
        ForumTab();
    };
}

namespace winrt::Pine::factory_implementation
{
    struct ForumTab : ForumTabT<ForumTab, implementation::ForumTab>
    {
    };
}
