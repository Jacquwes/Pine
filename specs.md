# API specifications

# Version 2

## How to establish a connection

| Emitter | Description |
| ------- | ------- |
| Client | Connect to the TCP listener |
| Server | Send a validation token |
| Client | Send the validation token ^ `0xF007CAFEC0C0CACA` |
| Client, Server | Start the keep alive process |
| Server | Send a hello message |
| Client | Send a hello message |
| Server | ACK |
| Client | Identify |
| Server | ACK |
| Client, Server | Start the communication |

\*  will send an ACK message for every message received from the client.

## Message format

| Bytes | Description |
|-------|-------------|
| 1     | Message type |
| 8     | Message Body length |
| 8    | Message Id |
| 0-XXXX   | Message data |

## Keep alive process

Client should send a keep alive message every 5 seconds.
If the server doesn't receive a keep alive message in 10 seconds, the connection will be closed.

## Error handling

If an erroneous message is sent by the client after its identification, the server will send an error message.
Before that, the server will close the connection.





## Message types

| Type   | Description                     | Sent by |
| ------ | ------------------------------- | --- |
| `0x00` | Invalid                         | Server |
| `0x01` | [ACK](#ack-message)             | Server |
| `0x02` | [Hello](#hello-message)         | Server, Client |
| `0x03` | [Identify](#identify-message)   | Client |
| `0x04` | [Keep alive](#keep-alive-message) | Client |
| `0x05` | [Send chat message](#send-chat-message) | Client |
| `0x06` | [Receive chat message](#send-chat-message) | Server |
| `0x07` | [Error](#error-message) | Server |

### ACK message

| Bytes | Description |
|-------|-------------|
| 64    | Acknowledged Message Id  |

### Hello message

| Bytes | Description |
|-------|-------------|
| 8     | API version |

Can throw [Error](#error-codes) `0x02` if the API version is not compatible.

### Identify message

| Bytes | Description |
|-------|-------------|
| 1	 | Username length (in bytes) |
| 3-32 | Username    |

Can throw [Error](#error-codes) `0x04` if the username length is not between 3 and 32.

### Keep alive message

| Bytes | Description |
|-------|-------------|
| 0     | No data |

### Send chat message

| Bytes | Description |
|-------|-------------|
| 2 | Chat message size length (in bytes) |
| 1-2000 | Chat message content |

Can throw [Error](#error-codes) `0x05` if the chat message length is not between 1 and 2000.

### Receive chat message

| Bytes | Description |
| ------- | ------- |
| 1 | Author username length (in bytes) |
| 3-32 | Author username |
| 2 | Chat message size length (in bytes) |
| 1-2000 | Chat message content |

### Error message

| Bytes | Description |
|-------|-------------|
| 1     | [Error code](#error-codes) |





## Error codes

| Code | Description |
|------|-------------|
| `0x00` | Invalid message |
| `0x01` | Invalid message type |
| `0x02` | Incompatible API version |
| `0x03` | Wrong message length |
| `0x04` | Wrong chat username length |
| `0x05` | Wrong chat message length |
