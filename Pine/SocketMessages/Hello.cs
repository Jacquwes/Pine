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

		public override byte[] Serialize()
		{
			return header.Serialize();
		}

		public static readonly UInt64 Size = 0;
	}
}
