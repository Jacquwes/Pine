using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using Microsoft.UI.Xaml.Controls.Primitives;
using Microsoft.UI.Xaml.Data;
using Microsoft.UI.Xaml.Input;
using Microsoft.UI.Xaml.Media;
using Microsoft.UI.Xaml.Navigation;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;
using Windows.Foundation;
using Windows.Foundation.Collections;

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace Pine.Views.Identify
{
	public sealed partial class IdentifyControl : UserControl
	{
		public IdentifyControl()
		{
			this.InitializeComponent();
		}

		private void LogInButton_Click(Object sender, RoutedEventArgs e)
		{
			OnLogIn?.Invoke(this, new LogInEventArgs(UsernameBlock.Text));
		}

		public event EventHandler<LogInEventArgs> OnLogIn;
	}

	public class LogInEventArgs : EventArgs
	{
		public LogInEventArgs(String username)
		{
			Username = username;
		}

		public String Username { get; set; }
	}
}
