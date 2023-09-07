#pragma once

#include <cstdint>
#include <memory>
#include <vector>

#include "snowflake.h"

namespace pine
{
	/// <summary>
	/// Minimum length of a chat message. It is used to prevent spam. Equals to 1.
	/// </summary>
	constexpr auto chat_message_min_length = 0x01;

	/// <summary>
	/// Maximum length of a chat message. It is used to prevent spam. Equals to 2000.
	/// </summary>
	constexpr auto chat_message_max_length = 2000;

	/// <summary>
	/// Current version of the protocol.
	/// </summary>
	constexpr auto current_version = 0x2;

	/// <summary>
	/// Minimum length of a username. Equals to 3.
	/// </summary>
	constexpr auto username_min_length = 0x03;

	/// <summary>
	/// Maximum length of a username. Equals to 32.
	/// </summary>
	constexpr auto username_max_length = 0x20;

	namespace socket_messages
	{
		enum message_type : uint8_t
		{
			invalid_message,
			acknowledge_message,
			hello_message,
			identify_message,
			keep_alive_message,
			send_chat_message,
			receive_chat_message,
			error_message,
		};

		/// <summary>
		/// Represents the header of a message. It contains the type of the message, the size of the body and the id of the message.
		/// It is located at the beginning of every message.
		/// </summary>
		struct message_header
		{
			message_header() = default;

			/// <summary>
			/// Creates a new message header from a buffer. The buffer must contain at least 13 bytes.
			/// The first byte is the type of the message, the next 8 bytes are the size of the body and the last 8 bytes are the id of the message.
			/// </summary>
			/// <param name="buffer"></param>
			explicit message_header(std::vector<uint8_t> const& buffer);

			/// <summary>
			/// Loads a message from a buffer. The buffer must contain at least 13 bytes.
			/// The first byte is the type of the message, the next 8 bytes are the size of the body and the last 8 bytes are the id of the message.
			/// </summary>
			void parse(std::vector<uint8_t> const& buffer);

			/// <summary>
			/// Serializes the message header to a buffer.
			/// </summary>
			std::vector<uint8_t> serialize() const;


			message_type type{};
			uint64_t body_size{};
			snowflake id{};

			/// <summary>
			/// The size of the message header in bytes.
			/// </summary>
			static uint64_t constexpr size = sizeof(message_type) + sizeof(uint64_t) + sizeof(id.value);
		};

		/// <summary>
		/// Represents a message. It contains a header and a body.
		/// </summary>
		struct message : std::enable_shared_from_this<message>
		{
			virtual ~message() = default;

			/// <summary>
			/// Returns the number of bytes of the body.
			/// </summary>
			virtual uint64_t get_body_size() const;

			/// <summary>
			/// Loads the body of the message from a buffer.
			/// </summary>
			/// <returns>True if the body was loaded successfully, false otherwise.</returns>
			virtual bool parse_body(std::vector<uint8_t> const& buffer);

			/// <summary>
			/// Serializes the body of the message to a buffer.
			/// </summary>
			virtual std::vector<uint8_t> serialize() const;

			/// <summary>
			/// Returns the header of the message.
			/// </summary>
			message_header header{};
		};
	}
}
