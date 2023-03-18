using System;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;
using System.Text;
using System.Threading.Tasks;

namespace Pine.SocketMessages
{
	internal class Hello : Message
	{
		public static readonly UInt64 CURRENT_VERSION = 0x0;

		public Hello()
		{
			header.Type = MessageType.Hello;
			header.BodySize = Size;
		}

		public Hello(Message m)
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
			buffer = buffer.Concat(header.Serialize()).ToArray();

			buffer = buffer.Concat(BitConverter.GetBytes(Version)).ToArray();

			return buffer;
		}

		public static readonly UInt64 Size = sizeof(UInt64);
		public UInt64 Version = 0x0;
	}
}
