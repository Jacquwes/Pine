using System;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;
using System.Text;
using System.Threading.Tasks;

namespace Pine.SocketMessages
{
	internal class LoginMessage : Message
	{
		public LoginMessage()
		{
			Header.Type = MessageType.Login;
			Header.BodySize = Size;
		}

		public override bool ParseBody(byte[] buffer)
		{
			if ((UInt64)buffer.LongLength != Size)
				return false;

			Username = Encoding.ASCII.GetString(buffer);
			return true;
		}

		public override byte[] Serialize()
		{
			byte[] buffer = Array.Empty<byte>();
			buffer = buffer.Concat(Header.Serialize()).ToArray();

			buffer = buffer.Concat(Encoding.ASCII.GetBytes(Username)).ToArray();
			Array.Resize(ref buffer, (int)MessageHeader.Size + (int)Size);

			return buffer;
		}

		public bool SetUsername(string username)
		{
			if (username.Length > UsernameMaxLength)
				return false;
			Username = username;
			return true;
		}

		public static readonly int UsernameMaxLength = 0x20;

		public string Username { get; private set; } = new('\0', UsernameMaxLength);
		public static readonly UInt64 Size = (UInt64)UsernameMaxLength;
	}
}
