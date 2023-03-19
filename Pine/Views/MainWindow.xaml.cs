using Microsoft.UI.Xaml;
using Pine.Client;
using Pine.Views.Identify;
using System;
using System.Threading.Tasks;
using Windows.UI.Popups;

namespace Pine
{
	public sealed partial class MainWindow : Window
	{

		public MainWindow()
		{
			InitializeComponent();

			MainFrame.Navigate(typeof(IdentifyPage));
		}
	}
}
