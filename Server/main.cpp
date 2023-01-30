import Server;
#pragma comment(lib, "ws2_32.lib")

int main()
{
	Server::Listener server;
	server.Run("45321");
}