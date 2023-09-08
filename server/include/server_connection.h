#pragma once

#include <thread>
#include <string_view>

#include <WinSock2.h>

#include "connection.h"
#include "coroutine.h"
#include "snowflake.h"

#pragma comment(lib, "ws2_32.lib")


namespace pine
{
    class server;

    class server_connection : public connection
    {
        friend class connection;

    public:
        server_connection(SOCKET& socket, server& server);

        async_task listen();

        async_task send_ack(snowflake id) const;

    private:
        async_operation<bool> establish_connection();
        async_operation<bool> validate_connection();
        async_operation<bool> check_version();
        async_operation<bool> identify();

        server& server_ref;
        std::jthread listen_thread;
    };
}