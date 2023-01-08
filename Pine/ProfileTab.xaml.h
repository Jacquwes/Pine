#pragma once

#include "ProfileTab.g.h"

namespace winrt::Pine::implementation
{
	struct ProfileTab : ProfileTabT<ProfileTab>
	{
		ProfileTab();
		
		hstring AvatarUri();
		void AvatarUri(hstring const& value);

		hstring BackgroundUri();
		void BackgroundUri(hstring const& value);

		hstring Id();
		void Id(hstring const& value);

		hstring Location();
		void Location(hstring const& value);

		hstring Signature();
		void Signature(hstring const& value);

		hstring Username();
		void Username(hstring const& value);
	};
}

namespace winrt::Pine::factory_implementation
{
	struct ProfileTab : ProfileTabT<ProfileTab, implementation::ProfileTab>
	{
	};
}
