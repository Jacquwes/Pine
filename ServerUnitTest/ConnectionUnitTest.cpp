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

			return std::make_shared<Connection>(sendingSocket, server);
		}

		Server server;
		std::jthread serverThread;
		std::mutex mutex;

		TEST_METHOD(EstablishConnection)
		{
			StartServer();

			std::shared_ptr<Connection> clientConnection = CreateConnection();

			std::condition_variable cv;
			bool finished = false;
			[&]() -> AsyncTask {
				co_await SwitchThread(clientConnection->GetThread());


				Logger::WriteMessage("Waiting for token");
				std::vector<uint8_t> token = co_await clientConnection->ReceiveRawMessage(sizeof(uint64_t));
				Assert::AreEqual(token.size(), sizeof(uint64_t), L"Wrong token size");
				Logger::WriteMessage("Received token");

				*std::bit_cast<uint64_t*>(token.data()) ^= 0xF007CAFEC0C0CA7E;

				co_await clientConnection->SendRawMessage(token);
				Logger::WriteMessage("Sent token");




				Logger::WriteMessage("Waiting for Hello");
				std::shared_ptr<SocketMessages::Message> message = co_await clientConnection->ReceiveMessage();
				Assert::AreEqual(static_cast<uint8_t>(message->header.messageType),
								 static_cast<uint8_t>(SocketMessages::MessageType::HelloMessage),
								 L"Wrong message type received instead of Hello");
				Logger::WriteMessage("Received Hello message");




				message = std::make_shared<SocketMessages::HelloMessage>();
				uint64_t expectedId = message->header.messageId;

				co_await clientConnection->SendMessage(message);
				Logger::WriteMessage("Sent Hello message");
				Logger::WriteMessage("Waiting for ACK");
				message = co_await clientConnection->ReceiveMessage();
				Assert::AreEqual(static_cast<uint8_t>(message->header.messageType),
								 static_cast<uint8_t>(SocketMessages::MessageType::AcknowledgeMessage),
								 L"Wrong message type instead of ACK");
				Logger::WriteMessage("Received ACK");

				uint64_t actualId =
					std::dynamic_pointer_cast<SocketMessages::AcknowledgeMessage>(message)
					->GetAcknowledgedMessageId();
				Assert::AreEqual(expectedId, actualId, L"Wrong message id received");




				message = std::make_shared<SocketMessages::IdentifyMessage>();
				std::dynamic_pointer_cast<SocketMessages::IdentifyMessage>(message)->SetUsername("Username");
				expectedId = message->header.messageId;

				co_await clientConnection->SendMessage(message);
				Logger::WriteMessage("Sent Identify message");
				message = co_await clientConnection->ReceiveMessage();
				Assert::AreEqual(static_cast<uint8_t>(message->header.messageType),
								 static_cast<uint8_t>(SocketMessages::MessageType::AcknowledgeMessage),
								 L"Wrong message type instead of ACK");
				Logger::WriteMessage("Received ACK");

				actualId = std::dynamic_pointer_cast<SocketMessages::AcknowledgeMessage>(message)
							   ->GetAcknowledgedMessageId();
				Assert::AreEqual(expectedId, actualId, L"Wrong message id received");



				closesocket(clientConnection->GetSocket());
				shutdown(clientConnection->GetSocket(), SD_BOTH);

				finished = true;
				cv.notify_one();

				co_return;
			}();

			std::unique_lock lock{ mutex };
			cv.wait(lock, [&finished] { return finished; });

			lock.unlock();
			lock.release();

			if (clientConnection->GetThread().joinable())
			{
				clientConnection->GetThread().request_stop();
				clientConnection->GetThread().join();
			}


			server.Stop();

			if (serverThread.joinable())
				serverThread.join();

			return;
		}
	};
}