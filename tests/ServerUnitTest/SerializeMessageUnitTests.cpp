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
			uint64_t expectedId = *std::bit_cast<uint64_t*>(&buffer[sizeof(header.messageType) + sizeof(header.bodySize)]);

			Assert::AreEqual(buffer.size(), SocketMessages::MessageHeader::size, L"Wrong header size");
			Assert::AreEqual(buffer[0], static_cast<uint8_t>(SocketMessages::MessageType::AcknowledgeMessage), L"Wrong header type");
			Assert::AreEqual(*std::bit_cast<uint64_t*>(&buffer[1]), header.bodySize, L"Wrong message type");
			Assert::AreEqual(expectedId, actualId, L"Wrong message content");
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

			Assert::AreEqual(expectedSize, actualSize, L"Wrong message size");
			Assert::AreEqual(expectedId, actualId, L"Wrong Id");
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

			Assert::AreEqual(expectedSize, actualSize, L"Wrong message size");
			Assert::AreEqual(expectedError, actualError, L"Wrong error");
		}

		TEST_METHOD(SerializeHelloMessage)
		{
			SocketMessages::HelloMessage message{};
			std::vector<uint8_t> buffer = message.Serialize();

			uint64_t expectedVersion = CurrentVersion;
			uint64_t actualVersion = *std::bit_cast<uint64_t*>(&buffer[SocketMessages::MessageHeader::size]);

			uint64_t expectedSize = SocketMessages::MessageHeader::size + message.GetBodySize();
			uint64_t actualSize = buffer.size();

			Assert::AreEqual(expectedSize, actualSize, L"Wrong message size");
			Assert::AreEqual(expectedVersion, actualVersion, L"Wrong version");
		}

		TEST_METHOD(SerializeIdentifyMessage)
		{
			SocketMessages::IdentifyMessage message{};

			std::string validUsername = "Username";
			std::string invalidUsername1 = "0";
			std::string invalidUsername2 = "012345678901234567890123456789012";

			Assert::IsTrue(message.SetUsername(validUsername), L"Valid username can't be set");
			Assert::IsFalse(message.SetUsername(invalidUsername1), L"Short username can be set");
			Assert::IsFalse(message.SetUsername(invalidUsername2), L"Long username can be set");

			std::vector<uint8_t> buffer = message.Serialize();

			uint64_t expectedSize = SocketMessages::MessageHeader::size + message.GetBodySize();
			uint64_t actualSize = buffer.size();

			Assert::AreEqual(expectedSize, actualSize, L"Wrong message size");
		}

		TEST_METHOD(SerializeKeepAliveMessage)
		{
			SocketMessages::KeepAliveMessage message{};
			std::vector<uint8_t> buffer = message.Serialize();

			uint64_t expectedSize = SocketMessages::MessageHeader::size + message.GetBodySize();
			uint64_t actualSize = buffer.size();

			Assert::AreEqual(expectedSize, actualSize, L"Wrong message size");
		}

		TEST_METHOD(SerializeReceiveChatMessage)
		{
			SocketMessages::ReceiveChatMessage message{};

			std::string validUsername = "Username";
			std::string invalidUsername1 = "0";
			std::string invalidUsername2 = "012345678901234567890123456789012";

			Assert::IsTrue(message.SetAuthorUsername(validUsername), L"Valid username can't be set");
			Assert::IsFalse(message.SetAuthorUsername(invalidUsername1), L"Short username can be set");
			Assert::IsFalse(message.SetAuthorUsername(invalidUsername2), L"Long username can be set");

			std::string validMessage = "Message";
			std::string invalidMessage1 = "";
			std::string invalidMessage2(2001, 'a');

			Assert::IsTrue(message.SetChatMessage(validMessage), L"Valid chat message can't be set");
			Assert::IsFalse(message.SetChatMessage(invalidMessage1), L"Short chat message can be set");
			Assert::IsFalse(message.SetChatMessage(invalidMessage2), L"Long username can be set");

			std::vector<uint8_t> buffer = message.Serialize();

			uint64_t expectedSize = SocketMessages::MessageHeader::size + message.GetBodySize();
			uint64_t actualSize = buffer.size();

			Assert::AreEqual(expectedSize, actualSize, L"Wrong message size");

			uint64_t expectedMessageSize = message.GetChatMessage().size();
			uint64_t actualMessageSize = *std::bit_cast<uint16_t*>(
				&buffer[SocketMessages::MessageHeader::size + sizeof(uint8_t) + validUsername.length()]
			);

			Assert::AreEqual(expectedMessageSize, actualMessageSize, L"Wrong chat message length");

			auto expectedAuthorSize = static_cast<uint8_t>(validUsername.size());
			uint8_t actualAuthorSize = *std::bit_cast<uint8_t*>(&buffer[SocketMessages::MessageHeader::size]);

			Assert::AreEqual(expectedAuthorSize, actualAuthorSize, L"Wrong author username length");
		}

		TEST_METHOD(SerializeSendChatMessage)
		{
			SocketMessages::SendChatMessage message{};

			std::string validMessage = "Message";
			std::string invalidMessage1 = "";
			std::string invalidMessage2(2001, 'a');

			Assert::IsTrue(message.SetChatMessage(validMessage), L"Valid chat message can't be set");
			Assert::IsFalse(message.SetChatMessage(invalidMessage1), L"Short chat message can be set");
			Assert::IsFalse(message.SetChatMessage(invalidMessage2), L"Long username can be set");

			std::vector<uint8_t> buffer = message.Serialize();

			uint64_t expectedSize = SocketMessages::MessageHeader::size + message.GetBodySize();
			uint64_t actualSize = buffer.size();

			Assert::AreEqual(expectedSize, actualSize, L"Wrong message size");

			uint64_t expectedMessageSize = message.GetChatMessage().size();
			uint64_t actualMessageSize = *std::bit_cast<uint16_t*>(
				&buffer[SocketMessages::MessageHeader::size]
			);

			Assert::AreEqual(expectedMessageSize, actualMessageSize, L"Wrong chat message length");
		}
	};
}
