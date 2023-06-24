#pragma warning(disable:4996)
#pragma comment(lib,"wsock32.lib")
#include"Header.h"

WSAData Wsadata;

SOCKET createSocket()
{
	SOCKET Socket = socket(AF_INET, SOCK_DGRAM, 0);
	u_long FAR arg = 1;
	ioctlsocket(Socket, FIONBIO, &arg);
	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(53);
	addr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	if (bind(Socket, (sockaddr*)&addr, sizeof(addr)) < 0)
	{
		printf("Fail to bind UDP port\n");
		exit(0);
	}
	else
		printf("Bind UDP port 53 ...OK!\n");
	return Socket;
}
