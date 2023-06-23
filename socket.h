#pragma once
#pragma comment(lib,"wsock32.lib")
#include<winsock2.h>
#include<stdio.h>
#define IP_LENGTH 16

WSAData Wsadata;
char serverIPAddress[IP_LENGTH] = "10.3.9.45";
char clientIPAddress[IP_LENGTH] = "127.0.0.1";

SOCKET CreateSocket(short sin_family, u_short sin_port,char* sin_addr)
{
	SOCKET Socket = socket(AF_INET, SOCK_DGRAM, 0);
	u_long FAR arg = 1;
	ioctlsocket(Socket, FIONBIO, &arg);
	sockaddr_in addr;
	addr.sin_family = sin_family;
	addr.sin_port = htons(sin_port);
	addr.sin_addr.S_un.S_addr = inet_addr(sin_addr);
	if (bind(Socket, (sockaddr*)&addr, sizeof(addr)) < 0)
	{
		printf("Fail to bind UDP port\n");
		exit(0);
	}
	else
		printf("Bind UDP port 53 ...OK!\n");
	return Socket;
}



