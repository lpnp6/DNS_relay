#pragma once
#pragma comment(lib,"wsock32.lib")
#include<winsock2.h>

#define Local_DNS_Address "127.0.0.1"
#define Server_DNS_Address "10.3.9.45"

SOCKET CreateSocket(short sin_family, u_short sin_port,const char* sin_addr)
{
	SOCKET Socket = socket(AF_INET, SOCK_DGRAM, 0);
	u_long FAR arg = 1;
	ioctlsocket(Socket, FIONBIO, &arg);
	sockaddr_in addr;
	addr.sin_family = sin_family;
	addr.sin_port = htons(sin_port);
	addr.sin_addr.S_un.S_addr = inet_addr(sin_addr);
	bind(Socket, (sockaddr*)&addr, sizeof(addr));
	return Socket;
}



