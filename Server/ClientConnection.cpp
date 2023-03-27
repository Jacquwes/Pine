#pragma once

#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>

#include "Connection.h"
#include "Snowflake.h"

ClientConnection::ClientConnection()
{}

bool ClientConnection::Connect()
{
	addrinfo hints{};
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	addrinfo* result = nullptr;
	int i = getaddrinfo("localhost", "45321", &hints, &result);
	if (i)
	{
		WSACleanup();
		return false;
	}

	m_socket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (m_socket == INVALID_SOCKET)
	{
		freeaddrinfo(result);
		WSACleanup();
		return false;
	}

	i = connect(m_socket, result->ai_addr, static_cast<int>(result->ai_addrlen));
	if (i == SOCKET_ERROR)
	{
		closesocket(m_socket);
		m_socket = INVALID_SOCKET;
		return false;
	}

	freeaddrinfo(result);

	return true;
}