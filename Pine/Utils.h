#pragma once

#include "MainWindow.xaml.h"
#include <type_traits>

using namespace winrt;
using namespace Microsoft::UI::Xaml;
using namespace Microsoft::UI::Xaml::Controls;

#define GetMainWindow(variable, root) \
auto variable{ root.try_as<Controls::Page>() };\
while (!variable && root)\
{\
	root = Media::VisualTreeHelper::GetParent(root);\
	variable = root.try_as<Controls::Page>();\
}\



/*
		try
		{
			auto window = child.as<Window>();
			return window.as<MainWindow>();
		}
		catch (hresult_error const& e)
		{
			if (e.code() == 0x80004002)
				return GetMainWindow(child.Parent().as<FrameworkElement>());
			throw e;
		}
*/