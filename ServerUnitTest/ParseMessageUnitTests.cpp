#include "CppUnitTest.h"

#include <bit>
#include <memory>

#include "Snowflake.h"
#include "SocketMessages.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace ServerUnitTest
{
	TEST_CLASS(ParseMessages)
	{
	public:

		TEST_METHOD(ParseACKMessage)
		{
			Snowflake messageId{};

			SocketMessages::AcknowledgeMessage ackMessage{ messageId };

			std::vector<uint8_t> messageBuffer = ackMessage.Serialize();
			std::vector<uint8_t> buffer(messageBuffer.begin() + SocketMessages::MessageHeader::size, messageBuffer.end());

			Assert::IsFalse(ackMessage.ParseBody({ 0, 3, 4 }), L"Invalid data can be parsed");
			Assert::IsTrue(ackMessage.ParseBody(buffer), L"Valid data can't be parsed");

			Assert::AreEqual(
				static_cast<uint64_t>(messageId),
				static_cast<uint64_t>(ackMessage.GetAcknowledgedMessageId()),
				L"Acknowledged message Id id wrong"
			);
		}

		TEST_METHOD(ParseErrorMessage)
		{
			SocketMessages::ErrorCode errorCode{ SocketMessages::ErrorCode::InvalidMessage };

			SocketMessages::ErrorMessage errorMessage{ errorCode };

			std::vector<uint8_t> messageBuffer = errorMessage.Serialize();
			std::vector<uint8_t> buffer(messageBuffer.begin() + SocketMessages::MessageHeader::size, messageBuffer.end());

			Assert::IsFalse(errorMessage.ParseBody({ 0, 3, 4 }), L"Invalid data can be parsed");
			Assert::IsTrue(errorMessage.ParseBody(buffer), L"Valid data can't be parsed");

			Assert::AreEqual(
				static_cast<uint8_t>(errorCode),
				static_cast<uint8_t>(errorMessage.GetErrorCode()),
				L"Error code is wrong"
			);
		}
	};
}
