#pragma once

#include <thread>
#include <string_view>

#include <asio/ip/tcp.hpp>

#include "connection.h"
#include "coroutine.h"
#include "snowflake.h"

namespace pine
{
    class server;

    /// @brief A connection to a client.
    class server_connection : public connection
    {
    public:
        /// @brief Construct a server connection with the given socket and server.
        server_connection(asio::ip::tcp::socket& socket, server& server);

        /// @brief Start listening for messages from the client.
        /// @return An asynchronous task completed when the connection has been closed.
        async_task listen();

        /// @brief Send an ack message to the client.
        /// @param id Id of the message to acknowledge.
        /// @return An asynchronous task completed when the message has been sent.
        async_task send_ack(snowflake id);

    private:
        /// @brief Establish a connection with the client.
        /// @return An asynchronous operation that returns true if the connection has been established.
        async_operation<bool> establish_connection();

        /// @brief Validate the connection with the client.
        /// @return An asynchronous operation that returns true if the connection has been validated.
        async_operation<bool> validate_connection();

        /// @brief Check the version of the client.
        /// @return An asynchronous operation that returns true if the version of the client is valid.
        async_operation<bool> check_version();

        /// @brief Identify the client. 
        /// @return An asynchronous operation that returns true if the client has been identified.
        async_operation<bool> identify();

        server& server_ref;
        std::jthread listen_thread;
    };
}