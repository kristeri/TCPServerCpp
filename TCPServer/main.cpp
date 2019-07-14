#include <iostream>
#include <WS2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

int main()
{
	// Winsock initialization
	WSADATA wsData;
	WORD ver = MAKEWORD(2, 2);

	int wsOk = WSAStartup(ver, &wsData);
	if (wsOk != 0)
	{
		std::cerr << "Failed to initialize winsock. Quitting program..." << std::endl;
		return 1;
	}

	// Create a listening socket
	SOCKET listening = socket(AF_INET, SOCK_STREAM, 0);
	if (listening == INVALID_SOCKET)
	{
		std::cerr << "Failed to create a listening socket. Quitting program...";
		return 1;
	}

	// Bind the IP address and port to a socket
	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(54000);
	hint.sin_addr.S_un.S_addr = INADDR_ANY;

	bind(listening, (sockaddr*)&hint, sizeof(hint));

	listen(listening, SOMAXCONN);

	sockaddr_in client;
	int clientSize = sizeof(client);

	SOCKET clientSocket = accept(listening, (sockaddr*)&client, &clientSize);
	if (clientSocket == INVALID_SOCKET)
	{
		std::cerr << "Failed to create a client socket. Quitting program...";
		return 1;
	}

	char host[NI_MAXHOST];
	char service[NI_MAXHOST];

	ZeroMemory(host, NI_MAXHOST);
	ZeroMemory(service, NI_MAXHOST);

	if (getnameinfo((sockaddr*)&client, sizeof(client), host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0)
	{
		std::cout << host << " connected on port " << service << std::endl;
	}
	else
	{
		inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
		std::cout << host << " connected on port " << ntohs(client.sin_port) << std::endl;
	}

	closesocket(listening);

	// Accept client message and echo back
	char buf[4096];

	while (true)
	{
		ZeroMemory(buf, 4096);
		int bytesReceived = recv(clientSocket, buf, 4096, 0);
		if (bytesReceived == SOCKET_ERROR)
		{
			std::cerr << "Socket error. Quitting program..." << std::endl;
			break;
		}
		if (bytesReceived == 0)
		{
			std::cout << "Client has disconnected " << std::endl;
			break;
		}
		
		send(clientSocket, buf, bytesReceived + 1, 0);
	}

	// Close client socket
	closesocket(clientSocket);

	// Winsock shutdown
	WSACleanup();

	return 0;
}