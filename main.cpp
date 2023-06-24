#include"Header.h"

int main(int argc,char* argv[])
{
	WSAStartup(MAKEWORD(2, 2), &Wsadata);
	setParameter(argc, argv);
	SOCKET serverSocket = CreateSocket(AF_INET, 53, clientIPAddress);
	SOCKET clientSocket = CreateSocket(AF_INET, 53, serverIPAddress);
}
