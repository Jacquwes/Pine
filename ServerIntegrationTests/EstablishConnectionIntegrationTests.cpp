#include "CppUnitTest.h"

#include <mutex>

#include "Connection.h"
#include "Coroutine.h"
#include "Message.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace ServerIntegrationTests
{
	TEST_CLASS(EstablishConnection)
	{
	public:



		TEST_METHOD(EstablishSingleConnection)
		{
			std::mutex mutex;

			[&mutex]() -> AsyncTask
			{
				ClientConnection client;

				Assert::IsTrue(client.Connect());

				std::lock_guard lock{ mutex };

				
				
				
				std::vector<uint8_t> validationToken = co_await client.ReceiveRawMessage(sizeof(uint64_t));

				Assert::AreEqual(sizeof(uint64_t), validationToken.size(), L"Wrong received token size");

				*std::bit_cast<uint64_t*>(validationToken.data()) ^= 0xF007CAFEC0C0CACA;

				co_await client.SendRawMessage(validationToken);

				std::shared_ptr<SocketMessages::Message> validationResponse = co_await client.ReceiveMessage();

				Assert::AreEqual(static_cast<uint8_t>(validationResponse->header.messageType),
								 static_cast<uint8_t>(SocketMessages::MessageType::HelloMessage),
								 L"Wrong message type received after validation");




				auto helloMessage = std::make_shared<SocketMessages::HelloMessage>();

				co_await client.SendMessage(helloMessage);

				std::shared_ptr<SocketMessages::Message> helloResponse = co_await client.ReceiveMessage();

				Assert::AreEqual(static_cast<uint8_t>(helloResponse->header.messageType),
								 static_cast<uint8_t>(SocketMessages::MessageType::AcknowledgeMessage),
								 L"Wrong message type received after hello");




				auto identifyMessage = std::make_shared<SocketMessages::IdentifyMessage>();
				identifyMessage->SetUsername("Username");

				co_await client.SendMessage(identifyMessage);

				std::shared_ptr<SocketMessages::Message> identifyResponse = co_await client.ReceiveMessage();

				Assert::AreEqual(static_cast<uint8_t>(identifyResponse->header.messageType),
								 static_cast<uint8_t>(SocketMessages::MessageType::AcknowledgeMessage),
								 L"Wrong message type received after hello");
			}();

			std::lock_guard lock{ mutex };
		}
	};
}
