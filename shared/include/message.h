#pragma once

#include <cstdint>
#include <memory>
#include <vector>

#include "snowflake.h"

namespace pine
{
	/// @brief Minimum length of a chat message.
	constexpr auto chat_message_min_length = 0x01;

	/// @brief Maximum length of a chat message.
	constexpr auto chat_message_max_length = 2000;

	/// @brief Current version of the protocol.
	constexpr auto current_version = 0x2;

	/// @brief Minimum length of a username.
	constexpr auto username_min_length = 0x03;

	/// @brief Maximum length of a username.
	constexpr auto username_max_length = 0x20;

	namespace socket_messages
	{
		/// @brief Type of a message.
		enum message_type : uint8_t
		{
			/// @brief Invalid message.
			/// @details This message should never be sent.
			INVALID_MESSAGE,

			/// @brief Acknowledge message.
			/// @details This message is sent as a response by the server
			/// to each message sent by the client.
			ACKNOWLEDGE_MESSAGE,

			/// @brief Hello message.
			/// @details This message is sent by the client and the server
			/// to confirm the connection has been established.
			HELLO_MESSAGE,

			/// @brief Identify message.
			/// @details This message is sent by the client to the server
			/// to identify itself.
			IDENTIFY_MESSAGE,

			/// @brief Keep alive message.
			/// @details This message is sent by the client every 5 seconds
			/// to keep the connection alive.
			KEEP_ALIVE_MESSAGE,

			/// @brief Send chat message.
			/// @details This message is sent by the client to the server
			/// to send a chat message.
			SEND_CHAT_MESSAGE,

			/// @brief Receive chat message.
			/// @details This message is sent by the server to the client
			/// to send a chat message.
			RECEIVE_CHAT_MESSAGE,

			/// @brief Error message.
			/// @details This message is sent by the server to the client
			/// to send an error message.
			ERROR_MESSAGE
		};

		/// @brief Represents a message header.
		/// @details The header contains the type of the message,
		/// the size of the body and the id of the message.
		/// It is meant to be placed at the beginning of a message.
		struct message_header
		{
			/// @brief Constructs an empty and invalid message header.
			message_header() = default;

			/// @brief Constructs a message header from a buffer.
			explicit message_header(std::vector<uint8_t> const& buffer);

			/// @brief Load the message header from a buffer.
			void parse(std::vector<uint8_t> const& buffer);

			/// @brief Serialize the message header to a buffer.
			std::vector<uint8_t> serialize() const;

			/// @brief Type of the message.
			message_type type{};

			/// @brief Size of the body of the message.
			uint64_t body_size{};

			/// @brief Id of the message.
			snowflake id{};

			/// @brief Size of the message header.
			static uint64_t constexpr size = sizeof(message_type) + sizeof(uint64_t) + sizeof(id.value);
		};

		/// @brief Represents a message.
		/// @details A message is composed of a header and a body.
		struct message : std::enable_shared_from_this<message>
		{
			/// @brief Default destructor.
			virtual ~message() = default;

			/// @brief Size of the body. 
			virtual uint64_t get_body_size() const;

			/// @brief Load the body of the message from a buffer.
			/// @return `true` if the body was successfully loaded, `false` otherwise.
			virtual bool parse_body(std::vector<uint8_t> const& buffer);

			/// @brief Serialize the message to a buffer.
			virtual std::vector<uint8_t> serialize() const;

			/// @brief Header of the message.
			message_header header{};
		};
	}
}
