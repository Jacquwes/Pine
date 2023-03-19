using Microsoft.UI.Xaml;
using Pine.Client;
using System;

namespace Pine
{
	public sealed partial class MainWindow : Window
	{
		readonly PineClient client;

		void OnClientConnected(object sender, bool connected)
		{
			if (connected)
				_ = client.ValidateConnection();
		}

		public MainWindow()
		{
			InitializeComponent();

			MainFrame.Navigate(typeof(IdentifyPage));
		}
	}
}
