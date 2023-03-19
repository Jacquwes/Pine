using Microsoft.UI;
using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using Microsoft.UI.Xaml.Controls.Primitives;
using Microsoft.UI.Xaml.Data;
using Microsoft.UI.Xaml.Input;
using Microsoft.UI.Xaml.Media;
using Microsoft.UI.Xaml.Navigation;
using Pine.SocketMessages;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;
using Windows.Foundation;
using Windows.Foundation.Collections;
using Windows.UI;

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace Pine.Views.Identify
{
	/// <summary>
	/// An empty page that can be used on its own or navigated to within a Frame.
	/// </summary>
	public sealed partial class IdentifyPage : Page
	{
		public IdentifyPage()
		{
			InitializeComponent();

			ConnectionStatusBlock.Text = "Connexion au serveur...";

			App.PineClient.OnConnected += PineClient_OnConnected;
			App.PineClient.OnConnectionFailed += PineClient_OnConnectionFailed;
		}

		private void PineClient_OnConnectionFailed(Object sender, Int32 e)
		{
			ConnectionStatusBlock.Text = "Impossible de se connecter au serveur : " + e;
			ConnectionStatusBlock.Foreground = new SolidColorBrush(Colors.Red);
		}

		private void PineClient_OnConnected(Object sender, Boolean e)
		{
			ConnectionStatusBlock.Text = "Connecté au serveur";
			ConnectionStatusBlock.Foreground = new SolidColorBrush(Colors.Green);

			IdentifyForm.OnLogIn += IdentifyForm_OnLogIn;
		}

		private void IdentifyForm_OnLogIn(Object sender, LogInEventArgs e)
		{
			LoginMessage loginMessage = new();
			loginMessage.SetUsername(e.Username);
			_ = App.PineClient.SendMessage(loginMessage);
		}
	}
}
