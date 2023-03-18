using Pine.SocketMessages;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Net.Sockets;
using System.Text;
using System.Threading.Tasks;

namespace Pine.Client
{
	internal class PineClient
	{
		private readonly TcpClient tcpClient;
		private NetworkStream stream;

		public PineClient()
		{
			tcpClient = new TcpClient();
		}

		~PineClient()
		{
			tcpClient.Close();
			tcpClient.Dispose();
		}

		public async Task<bool> ConnectAsync(string host, int port)
		{
			await tcpClient.ConnectAsync(host, port);

			if (tcpClient.Connected)
				stream = tcpClient.GetStream();

			OnConnected?.Invoke(this, tcpClient.Connected);

			return tcpClient.Connected;
		}

		public async Task<byte[]> ReceiveRawMessage(UInt64 size)
		{
			byte[] buffer = new byte[size];
			await stream.ReadAsync(buffer);
			return buffer;
		}

		public async Task SendRawMessage(byte[] buffer)
		{
			await tcpClient.Client.SendAsync(buffer,SocketFlags.None);
		}

		public async Task<bool> ValidateConnection()
		{
			byte[] buffer = await ReceiveRawMessage(8);

			UInt64 key = BitConverter.ToUInt64(buffer) ^ 0xF007CAFEC0C0CA7E;

			await SendRawMessage(BitConverter.GetBytes(key));

			byte[] hello = await ReceiveRawMessage(9);
			MessageHeader messageHeader = new MessageHeader(hello);
			if (messageHeader.Type != MessageType.Hello)
			{
				OnValidated?.Invoke(this, false);
				return false;
			}

			OnValidated?.Invoke(this, true);
			return true;
		}

		public event EventHandler<bool> OnConnected;
		public event EventHandler<bool> OnValidated;
	}
}
