#include "CppUnitTest.h"

#include <chrono>
#include <condition_variable>
#include <coroutine>
#include <stdexcept>
#include <thread>
#include <WinSock2.h>
#include <WS2tcpip.h>


#include "Connection.h"
#include "Coroutine.h"
#include "Server.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace std::chrono_literals;

namespace ServerUnitTest
{
	TEST_CLASS(ConnectionUnitTest)
	{
	public:
		void StartServer()
		{
			serverThread = std::jthread{ [this]() { server.Run("45321"); } };

			std::unique_lock lock(mutex);
			server.m_cv.wait(lock, [this] { return !server.m_stop; });

			lock.unlock();
			lock.release();
		}

		std::shared_ptr<Connection> CreateConnection()
		{
			addrinfo hints{};
			hints.ai_family = AF_INET;
			hints.ai_socktype = SOCK_STREAM;
			hints.ai_protocol = IPPROTO_TCP;

			addrinfo* result = nullptr;
			int iResult = getaddrinfo("localhost", "45321", &hints, &result);
			Assert::AreEqual(iResult, 0, L"Can't get address info");

			SOCKET sendingSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
			Assert::AreNotEqual(sendingSocket, INVALID_SOCKET, L"Can't start test socket");

			iResult = connect(sendingSocket, result->ai_addr, static_cast<int>(result->ai_addrlen));
			Assert::AreEqual(iResult, 0, L"Can't connect to the server");

			freeaddrinfo(result);

			return std::make_shared<Connection>(sendingSocket, server, false);
		}

		AsyncTask ConnectClient(std::shared_ptr<Connection> client, std::string username) const
		{
			std::vector<uint8_t> token = co_await client->ReceiveRawMessage(sizeof(uint64_t));
			*std::bit_cast<uint64_t*>(token.data()) ^= 0xF007CAFEC0C0CA7E;
			co_await client->SendRawMessage(token);

			// Hello
			std::shared_ptr<SocketMessages::Message> message = co_await client->ReceiveMessage();

			message = std::make_shared<SocketMessages::HelloMessage>();
			// Hello
			co_await client->SendMessage(message);
			// ACK
			message = co_await client->ReceiveMessage();

			message = std::make_shared<SocketMessages::IdentifyMessage>();
			std::dynamic_pointer_cast<SocketMessages::IdentifyMessage>(message)->SetUsername(username);

			// Identify
			co_await client->SendMessage(message);
			// ACK
			message = co_await client->ReceiveMessage();
		}

		Server server;
		std::jthread serverThread;
		std::mutex mutex;

		TEST_METHOD(EstablishConnection)
		{
			StartServer();

			std::condition_variable cv;
			bool finished = false;

			[&]() -> AsyncTask {
				std::shared_ptr<Connection> clientConnection = CreateConnection();

				co_await SwitchThread(clientConnection->GetThread());



				std::vector<uint8_t> token = co_await clientConnection->ReceiveRawMessage(sizeof(uint64_t));
				Assert::AreEqual(token.size(), sizeof(uint64_t), L"Wrong token size");

				*std::bit_cast<uint64_t*>(token.data()) ^= 0xF007CAFEC0C0CA7E;

				co_await clientConnection->SendRawMessage(token);




				std::shared_ptr<SocketMessages::Message> message = co_await clientConnection->ReceiveMessage();
				Assert::AreEqual(static_cast<uint8_t>(message->header.messageType),
								 static_cast<uint8_t>(SocketMessages::MessageType::HelloMessage),
								 L"Wrong message type received instead of Hello");




				message = std::make_shared<SocketMessages::HelloMessage>();
				uint64_t expectedId = message->header.messageId;

				co_await clientConnection->SendMessage(message);
				message = co_await clientConnection->ReceiveMessage();
				Assert::AreEqual(static_cast<uint8_t>(message->header.messageType),
								 static_cast<uint8_t>(SocketMessages::MessageType::AcknowledgeMessage),
								 L"Wrong message type instead of ACK");

				uint64_t actualId =
					std::dynamic_pointer_cast<SocketMessages::AcknowledgeMessage>(message)
					->GetAcknowledgedMessageId();
				Assert::AreEqual(expectedId, actualId, L"Wrong message id received");




				message = std::make_shared<SocketMessages::IdentifyMessage>();
				std::dynamic_pointer_cast<SocketMessages::IdentifyMessage>(message)->SetUsername("Username");
				expectedId = message->header.messageId;

				co_await clientConnection->SendMessage(message);
				message = co_await clientConnection->ReceiveMessage();
				Assert::AreEqual(static_cast<uint8_t>(message->header.messageType),
								 static_cast<uint8_t>(SocketMessages::MessageType::AcknowledgeMessage),
								 L"Wrong message type instead of ACK");

				actualId = std::dynamic_pointer_cast<SocketMessages::AcknowledgeMessage>(message)
					->GetAcknowledgedMessageId();
				Assert::AreEqual(expectedId, actualId, L"Wrong message id received");



				closesocket(clientConnection->GetSocket());
				shutdown(clientConnection->GetSocket(), SD_BOTH);

				finished = true;
				cv.notify_one();

				co_return;
			}();

			{
				std::unique_lock lock{ mutex };
				cv.wait(lock, [&finished] { return finished; });
			}

			server.Stop();
		}

		TEST_METHOD(SendChatMessage)
		{
			StartServer();

			std::condition_variable cv;
			std::pair<bool, bool> finished = { false, false };
			std::pair<bool, bool> connected = { false, false };

			std::string chatContent = "Hello world";
			std::string senderUsername = "Sender";
			
			std::mutex readyMutex;



			[&]() -> AsyncTask {
				auto receiverConnection = CreateConnection();

				co_await SwitchThread(receiverConnection->GetThread());

				co_await ConnectClient(receiverConnection, "Receiver");

				connected.second = true;
				cv.notify_all();

				auto&& message = co_await receiverConnection->ReceiveMessage();
				Assert::AreEqual(static_cast<uint8_t>(message->header.messageType),
								 static_cast<uint8_t>(SocketMessages::MessageType::SendChatMessage),
								 L"Wrong message type received instead of SendChatMessage");
				Assert::AreEqual(std::dynamic_pointer_cast<SocketMessages::ReceiveChatMessage>(message)->GetChatMessage(),
								 chatContent,
								 L"Wrong message received");
				Assert::AreEqual(std::dynamic_pointer_cast<SocketMessages::ReceiveChatMessage>(message)->GetAuthorUsername(),
								 senderUsername,
								 L"Wrong message received");


				closesocket(receiverConnection->GetSocket());
				shutdown(receiverConnection->GetSocket(), SD_BOTH);

				finished.second = true;
				cv.notify_all();
			}();



			[&]() -> AsyncTask {
				auto senderConnection = CreateConnection();

				co_await SwitchThread(senderConnection->GetThread());

				co_await ConnectClient(senderConnection, senderUsername);

				{
					std::unique_lock lock{ readyMutex };
					cv.wait(lock, [&connected] { return connected.second; });
				}

				auto message = std::make_shared<SocketMessages::SendChatMessage>();
				message->SetChatMessage(chatContent);
				co_await senderConnection->SendMessage(message);

				auto&& Ack = co_await senderConnection->ReceiveMessage();
				Assert::AreEqual(static_cast<uint8_t>(Ack->header.messageType),
								 static_cast<uint8_t>(SocketMessages::MessageType::AcknowledgeMessage),
								 L"Wrong message type received instead of ACK");

				closesocket(senderConnection->GetSocket());
				shutdown(senderConnection->GetSocket(), SD_BOTH);

				finished.first = true;
				cv.notify_all();
			}();



			{
				std::unique_lock lock{ mutex };
				cv.wait(lock, [&finished] { return finished.first && finished.second; });
			}

			server.Stop();
		}
	};
}