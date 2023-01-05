#pragma once

#include "SettingsTab.g.h"

namespace winrt::Pine::implementation
{
    struct SettingsTab : SettingsTabT<SettingsTab>
    {
        SettingsTab();
    };
}

namespace winrt::Pine::factory_implementation
{
    struct SettingsTab : SettingsTabT<SettingsTab, implementation::SettingsTab>
    {
    };
}
