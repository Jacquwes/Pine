using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Pine.SocketMessages
{
	internal class Hello : Message
	{
		public Hello()
		{
			header.Type = MessageType.Hello;
		}

		public override bool ParseBody(byte[] buffer)
		{
			if ((UInt64)buffer.LongLength != Size + MessageHeader.Size)
				return false;

			Version = BitConverter.ToUInt64(buffer, 0);
			return true;
		}

		public override byte[] Serialize()
		{
			byte[] buffer = {};
			buffer = buffer.Concat(header.Serialize()).ToArray();

			buffer = buffer.Concat(BitConverter.GetBytes(Version)).ToArray();

			return buffer;
		}

		public static readonly UInt64 Size = sizeof(UInt64);
		public UInt64 Version = 0x0;
	}
}
