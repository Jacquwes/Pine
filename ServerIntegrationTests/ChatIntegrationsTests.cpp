#include "CppUnitTest.h"

#include <mutex>

#include "Connection.h"
#include "Coroutine.h"
#include "EstablishConnection.h"
#include "Message.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace ServerIntegrationTests
{
	TEST_CLASS(ChatIntegrationTests)
	{
		TEST_METHOD(SendChatMessage)
		{
			std::mutex mutex;

			[&mutex]() -> AsyncTask
			{
				std::lock_guard lock{ mutex };

				ClientConnection sender;
				ClientConnection receiver;

				co_await EstablishConnection(sender);
				co_await EstablishConnection(receiver);

				auto sendChat = std::make_shared<SocketMessages::SendChatMessage>();
				sendChat->SetChatMessage("Hello world!");

				co_await sender.SendMessage(sendChat);

				std::shared_ptr<SocketMessages::Message> receivedChat = co_await receiver.ReceiveMessage();

				Assert::AreEqual(static_cast<uint8_t>(receivedChat->header.messageType),
								 static_cast<uint8_t>(SocketMessages::MessageType::ReceiveChatMessage),
								 L"Receiver didn't receive message");
				
				Assert::AreEqual(std::dynamic_pointer_cast<SocketMessages::ReceiveChatMessage>(receivedChat)->GetChatMessage(),
								 std::string("Hello world!"),
								 L"Receiver didn't receive correct message");

				Assert::AreEqual(std::dynamic_pointer_cast<SocketMessages::ReceiveChatMessage>(receivedChat)->GetAuthorUsername(),
								 sender.GetUser().GetUsername(),
								 L"Received message from wrong username");
			}();

			std::lock_guard lock{ mutex };
		}
	};
}