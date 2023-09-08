#include <iostream>
#include <memory>
#include <random>
#include <string>
#include <type_traits>
#include <vector>

#include "connection.h"
#include "coroutine.h"
#include "server.h"
#include "server_exception.h"
#include "socket_messages.h"



Connection::~Connection()
{
	std::cout << "Closing client connection: " << m_id << std::endl;

	if (m_socket != INVALID_SOCKET)
	{
		shutdown(m_socket, SD_BOTH);
		closesocket(m_socket);
	}
}



AsyncOperation<std::vector<uint8_t>> Connection::ReceiveRawMessage(uint64_t const& bufferSize) const
{
	std::vector<uint8_t> buffer(bufferSize, 0);

	if (!bufferSize)
		co_return buffer;

	int n = recv(m_socket, std::bit_cast<char*>(buffer.data()), static_cast<int>(bufferSize), 0);
	if (n == SOCKET_ERROR)
	{
		int error = WSAGetLastError();

		if (error == WSAECONNRESET || error == WSAECONNABORTED)
		{
			co_return buffer;
		}

		throw ServerException{ "Failed to receive message: " + WSAGetLastError() };
	}

	buffer.resize(n);
	co_return buffer;
}



AsyncTask Connection::SendRawMessage(std::vector<uint8_t> const& buffer) const
{
	int n = send(m_socket, std::bit_cast<char*>(buffer.data()), static_cast<int>(buffer.size()), 0);
	if (n == SOCKET_ERROR)
	{
		throw ServerException{ "Failed to send message: " + WSAGetLastError() };
	}

	co_return;
}



AsyncOperation<std::shared_ptr<SocketMessages::Message>> Connection::ReceiveMessage()
{
	auto message = std::make_shared<SocketMessages::Message>();

	std::vector<uint8_t> receivedMessage = co_await ReceiveRawMessage(SocketMessages::MessageHeader::size);
	if (receivedMessage.size() != SocketMessages::MessageHeader::size)
		co_return message;

	SocketMessages::MessageHeader header{ receivedMessage };
	if (header.messageType == SocketMessages::MessageType::InvalidMessage)
		co_return message;

	std::vector<uint8_t> body = co_await ReceiveRawMessage(header.bodySize);


	switch (header.messageType)
	{
		using enum SocketMessages::MessageType;

	case AcknowledgeMessage:
		message = std::make_shared<SocketMessages::AcknowledgeMessage>();

		if (!std::dynamic_pointer_cast<SocketMessages::AcknowledgeMessage>(message)->ParseBody(body))
			co_return message;
		break;

	case HelloMessage:
		message = std::make_shared<SocketMessages::HelloMessage>();

		if (!std::dynamic_pointer_cast<SocketMessages::HelloMessage>(message)->ParseBody(body))
			co_return message;
		break;

	case IdentifyMessage:
		message = std::make_shared<SocketMessages::IdentifyMessage>();

		if (!std::dynamic_pointer_cast<SocketMessages::IdentifyMessage>(message)->ParseBody(body))
			co_return message;
		break;

	case KeepAliveMessage:
		message = std::make_shared<SocketMessages::KeepAliveMessage>();

		if (!std::dynamic_pointer_cast<SocketMessages::KeepAliveMessage>(message)->ParseBody(body))
			co_return message;
		break;

	case SendChatMessage:
		message = std::make_shared<SocketMessages::SendChatMessage>();

		if (!std::dynamic_pointer_cast<SocketMessages::SendChatMessage>(message)->ParseBody(body))
			co_return message;
		break;

	case ReceiveChatMessage:
		message = std::make_shared<SocketMessages::ReceiveChatMessage>();

		if (!std::dynamic_pointer_cast<SocketMessages::ReceiveChatMessage>(message)->ParseBody(body))
			co_return message;
		break;

	default:
		co_return message;
		break;
	}


	message->header = header;

	if (dynamic_cast<ServerConnection*>(this) != nullptr)
		co_await ((ServerConnection*)this)->SendAck(header.messageId);

	co_return message;
}



AsyncTask Connection::SendMessage(std::shared_ptr<SocketMessages::Message> const& message) const
{
	std::vector<uint8_t> buffer = message->Serialize();

	co_await SendRawMessage(buffer);
}
