#include "Server.h"
#include "Connection.h"
#include "Coroutine.h"
#include "SocketMessages.h"
#include "Snowflake.h"

#include <iostream>
#include <memory>

AsyncTask Server::OnMessage(std::shared_ptr<Connection> const& client, std::shared_ptr<SocketMessages::Message> const& message)
{
	std::cout << "  Message received from client: " << std::dec << client->GetId() << std::endl;

	co_return;
}
