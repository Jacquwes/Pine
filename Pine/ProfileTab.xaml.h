#pragma once

#include "ProfileTab.g.h"

namespace winrt::Pine::implementation
{
	struct ProfileTab : ProfileTabT<ProfileTab>
	{
		ProfileTab();

		hstring BackgroundUri();
		void BackgroundUri(hstring const& value);
	};
}

namespace winrt::Pine::factory_implementation
{
	struct ProfileTab : ProfileTabT<ProfileTab, implementation::ProfileTab>
	{
	};
}
