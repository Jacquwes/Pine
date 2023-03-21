#include "CppUnitTest.h"

#include <bit>
#include <memory>

#include "SocketMessages.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace ServerUnitTest
{
	TEST_CLASS(SerializeMessages)
	{
	public:
		
		TEST_METHOD(SerializeHeader)
		{
			SocketMessages::MessageHeader header;
			header.messageType = SocketMessages::MessageType::AcknowledgeMessage;
			header.bodySize = SocketMessages::AcknowledgeMessage().GetBodySize();
			header.messageId = Snowflake();
			std::vector<uint8_t>  buffer = header.Serialize();

			uint64_t actualId = header.messageId;
			uint64_t expectedId;
			std::memcpy(&expectedId, &buffer[sizeof(header.messageType) + sizeof(header.bodySize)], sizeof(expectedId));

			Assert::AreEqual(buffer.size(), SocketMessages::MessageHeader::size);
			Assert::AreEqual(buffer[0], static_cast<uint8_t>(SocketMessages::MessageType::AcknowledgeMessage));
			Assert::AreEqual(*std::bit_cast<uint64_t*>(&buffer[1]), header.bodySize);
			Assert::AreEqual(expectedId, actualId);
		}

		TEST_METHOD(SerializeACKMessage)
		{
			Snowflake acknowledgedMessageId;

			SocketMessages::AcknowledgeMessage message{ acknowledgedMessageId };
			std::vector<uint8_t> buffer = message.Serialize();

			uint64_t expectedSize = SocketMessages::MessageHeader::size + message.GetBodySize();
			uint64_t actualSize = buffer.size();

			uint64_t expectedId = acknowledgedMessageId;
			uint64_t actualId = *std::bit_cast<uint64_t*>(&buffer[SocketMessages::MessageHeader::size]);

			Assert::AreEqual(expectedSize, actualSize);
			Assert::AreEqual(expectedId, actualId);
		}

		TEST_METHOD(SerializeErrorMessage)
		{
			auto errorCode = SocketMessages::ErrorCode::InvalidMessage;

			SocketMessages::ErrorMessage message{ errorCode };
			std::vector<uint8_t> buffer = message.Serialize();

			uint64_t expectedSize = SocketMessages::MessageHeader::size + message.GetBodySize();
			uint64_t actualSize = buffer.size();

			auto expectedError = static_cast<uint8_t>(errorCode);
			uint8_t actualError = *std::bit_cast<uint8_t*>(&buffer[SocketMessages::MessageHeader::size]);

			Assert::AreEqual(expectedSize, actualSize);
			Assert::AreEqual(expectedError, actualError);
		}

		TEST_METHOD(SerializeHelloMessage)
		{
			SocketMessages::HelloMessage message{};
			std::vector<uint8_t> buffer = message.Serialize();

			uint64_t expectedVersion = CurrentVersion;
			uint64_t actualVersion = *std::bit_cast<uint64_t*>(&buffer[SocketMessages::MessageHeader::size]);

			Assert::AreEqual(expectedVersion, actualVersion);
		}

		TEST_METHOD(SerializeIdentifyMessage)
		{
			SocketMessages::IdentifyMessage message{};

			std::string validUsername = "Username";
			std::string invalidUsername1 = "0";
			std::string invalidUsername2 = "012345678901234567890123456789012";

			Assert::IsTrue(message.SetUsername(validUsername));
			Assert::IsFalse(message.SetUsername(invalidUsername1));
			Assert::IsFalse(message.SetUsername(invalidUsername2));

			std::vector<uint8_t> buffer = message.Serialize();
			
			uint64_t expectedSize = SocketMessages::MessageHeader::size + message.GetBodySize();
			uint64_t actualSize = buffer.size();

			Assert::AreEqual(expectedSize, actualSize);
		}
	};
}
