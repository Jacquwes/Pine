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
	public class PineClient
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
			try
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
			catch (SocketException e)
			{
				OnConnectionFailed?.Invoke(this, (Int32)e.SocketErrorCode);

				return false;
			}
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
				HelloMessage hello = new();

				if (!hello.ParseBody(body))
					return message;

				message = hello;
			}
			else
				return message;

			message.Header = messageHeader;

			return message;
		}

		public async Task SendMessage(Message message)
		{
			await SendRawMessage(message.Serialize());
		}

		public async Task<bool> CheckVersion()
		{
			Message helloReceived = await ReceiveMessage();

			if (helloReceived.Header.Type != MessageType.Hello)
				return false;

			if (((HelloMessage)helloReceived).Version != HelloMessage.CURRENT_VERSION)
				return false;

			HelloMessage hello = new()
			{
				Version = HelloMessage.CURRENT_VERSION
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
		public event EventHandler<int> OnConnectionFailed;
	}
}
