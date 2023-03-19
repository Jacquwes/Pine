# Communication session specifications

# Version 1

1. Client connects to the server
2. Server sends a validation token to the client
3. Client musts answer with `token ^ 0xF007CAFEC0C0CA7E`
	* If the answer is incorrect, the connection will be closed
4. Server sends a hello message, including the version of the specification
	* If the version is not equal, client should close the connection
5. Client sends a hello message, including the version of the specification
	* If the version is not equal, server should close the connection
6. Client sends an identify message
7. Communication can start

## Message format

| Bytes | Description |
|-------|-------------|
| 1     | Message type |
| 8     | Message Body length |
| 0-XXXX   | Message data |

## Message types

| Type | Description |
| ---- | ----------- |
| `0x00` | Invalid |
| `0x01` | Hello |

### Hello message

| Bytes | Description |
|-------|-------------|
| 8     | Specification version |

### Identify message

| Bytes | Description |
|-------|-------------|
| 36     | Username |