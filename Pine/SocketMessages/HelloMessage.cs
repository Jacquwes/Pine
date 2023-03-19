using System;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;
using System.Text;
using System.Threading.Tasks;

namespace Pine.SocketMessages
{
	public class HelloMessage : Message
	{
		public static readonly UInt64 CURRENT_VERSION = 0x0;

		public HelloMessage()
		{
			Header.Type = MessageType.Hello;
			Header.BodySize = Size;
		}

		public HelloMessage(Message m)
		{
			foreach (FieldInfo prop in m.GetType().GetFields())
				GetType().GetField(prop.Name).SetValue(this, prop.GetValue(m));

			foreach (PropertyInfo prop in m.GetType().GetProperties())
				GetType().GetProperty(prop.Name).SetValue(this, prop.GetValue(m, null), null);
		}

		public override bool ParseBody(byte[] buffer)
		{
			if ((UInt64)buffer.LongLength != Size)
				return false;

			Version = BitConverter.ToUInt64(buffer, 0);
			return true;
		}

		public override byte[] Serialize()
		{
			byte[] buffer = Array.Empty<byte>();
			buffer = buffer.Concat(Header.Serialize()).ToArray();

			buffer = buffer.Concat(BitConverter.GetBytes(Version)).ToArray();

			return buffer;
		}

		public static readonly UInt64 Size = sizeof(UInt64);
		public UInt64 Version { get; set; } = 0;
	}
}
