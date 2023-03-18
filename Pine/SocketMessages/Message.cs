using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Pine.SocketMessages
{
	enum MessageType : byte
	{
		Invalid,
		Hello,
		Login
	}

	internal class MessageHeader
	{
		public MessageHeader()
		{}

		public MessageHeader(byte[] buffer)
		{
			Parse(buffer);
		}

		public void Parse(byte[] buffer)
		{
			Size = BitConverter.ToUInt64(buffer, 1);

			byte messageType = buffer[0];
			switch (messageType)
			{
				case (byte)MessageType.Hello:
					Type = MessageType.Hello;
					if (Size != Hello.Size)
						Type = MessageType.Invalid;
					break;
				case (byte)MessageType.Login:
					Type = MessageType.Login;
					break;
				default:
					Type = MessageType.Invalid;
					break;
			}
		}

		public byte[] Serialize()
		{
			byte[] buffer = new byte[9];
			buffer[0] = (byte)Type;
			Array.Copy(BitConverter.GetBytes(Size), 0, buffer, 1, 8);
			return buffer;
		}

		public MessageType Type { get; set; }
		public UInt64 Size { get; set; }
	}

	class Message
	{
		public MessageHeader header;

		public virtual bool ParseBody(byte[] buffer)
		{
			return false;
		}

		public virtual byte[] Serialize()
		{
			return new byte[9];
		}
	}
}
