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
			else
				return false;

			await ValidateConnection();

			if (!await CheckVersion())
				return false;

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
			await tcpClient.Client.SendAsync(buffer, SocketFlags.None);
		}

		public async Task<Message> ReceiveMessage()
		{
			Message message = new();

			byte[] header = await ReceiveRawMessage(MessageHeader.Size);
			MessageHeader messageHeader = new(header);
			
			if (messageHeader.Type == MessageType.Invalid)
				return message;

			byte[] body = await ReceiveRawMessage(messageHeader.BodySize);

			if (messageHeader.Type == MessageType.Hello)
			{
				Hello hello = new();

				if (!hello.ParseBody(body))
					return message;

				message = hello;
			}
			else
				return message;

			message.header = messageHeader;

			return message;
		}

		public async Task<bool> CheckVersion()
		{
			Message helloReceived = await ReceiveMessage();

			if (helloReceived.header.Type != MessageType.Hello)
				return false;

			if (((Hello)helloReceived).Version != Hello.CURRENT_VERSION)
				return false;

			Hello hello = new()
			{
				Version = Hello.CURRENT_VERSION
			};

			await SendRawMessage(hello.Serialize());

			return true;
		}

		public async Task ValidateConnection()
		{
			byte[] buffer = await ReceiveRawMessage(8);

			UInt64 key = BitConverter.ToUInt64(buffer) ^ 0xF007CAFEC0C0CA7E;

			await SendRawMessage(BitConverter.GetBytes(key));
		}

		public event EventHandler<bool> OnConnected;
	}
}
