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

			client = new PineClient();
			client.OnConnected += OnClientConnected;

			_ = client.ConnectAsync("127.0.0.1", 45321);
		}
	}
}
