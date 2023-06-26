#pragma warning (disable:4996)
#include"Header.h"

extern ID_trans id_trans_table[AMOUNT];	//ID转换表
extern int IDcount;			//转换表中的条目个数
extern char Url[lengthURL];					//域名
extern SYSTEMTIME TimeOfSys;                     //系统时间
extern int Day, Hour, Minute, Second, Milliseconds;//保存系统时间的变量
extern map<string, string>ip_domain_Map;//DNS域名解析表
extern char LocalDNSAddress[IPLength];
extern char DefDNSAddress[IPLength];
extern char filePath[MAX_FILE_LENGTH];
extern int DebugLevel;
extern map<char*, cacheInfo>cache;//cache表
extern bool needAdd;


thread threadpool[threadNum];
bool isBusy[threadNum] = {false};

int main(int argc, char** argv)
{
	setParameter(argc, argv);
	//初始化 DLL
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	//创建套接字
	SOCKET servSock = socket(AF_INET, SOCK_DGRAM, 0);
	SOCKET localSock = socket(AF_INET, SOCK_DGRAM, 0);

	//将套接口都设置为非阻塞
	int unBlock = 1;
	ioctlsocket(servSock, FIONBIO, (u_long FAR*) & unBlock);//将外部套街口设置为非阻塞
	ioctlsocket(localSock, FIONBIO, (u_long FAR*) & unBlock);//将本地套街口设置为非阻塞

	//绑定套接字
	SOCKADDR_IN serverName, clientName, localName;	//本地DNS、外部DNS和请求端三个网络套接字地址
	localName.sin_family = AF_INET;
	localName.sin_port = htons(PORT);
	localName.sin_addr.s_addr = inet_addr(LocalDNSAddress);
	serverName.sin_family = AF_INET;
	serverName.sin_port = htons(PORT);
	serverName.sin_addr.s_addr = inet_addr(DefDNSAddress);

	//绑定本地服务器地址
	if (bind(localSock, (SOCKADDR*)&localName, sizeof(localName)))
	{
		printf("Bind 53 port failed.\n");
		exit(-1);
	}
	printf("Bind 53 port success.\n");

	char sendBuf[BUFSIZE]; //发送缓存
	
	
	

	read(filePath);
	
	thread timerr = thread(threadFunc);//更新cache里的ttl
	timerr.detach();


		

	//下面是服务器的具体操作
	while (1)
	{

		char recvBuf[BUFSIZE]; //接收缓存
		int client_len, iRecv;
		client_len = sizeof(clientName);
		memset(recvBuf, 0, BUFSIZE); //将接收缓存先置为全0

		//接收DNS请求
		iRecv = recvfrom(localSock, recvBuf, sizeof(recvBuf), 0, (SOCKADDR*)&clientName, &client_len);
		//错误反馈
		if (iRecv == SOCKET_ERROR)
		{
			continue; 
		}
		else if (iRecv == 0)
		{
			break; //循环结束
		}
		else
		{
			//分配线程。
			bool allbusy = true;
			for(int i=0;i<threadNum;i++)
			{
				if(isBusy[i]==false)
				{
					threadpool[i] = thread(newthread, i,servSock, localSock, serverName, clientName, (recvBuf), (iRecv), (client_len));;
					threadpool[i].detach();
					allbusy = false;
					break;
				}
			}
			if(allbusy)
			{
				printf("线程无空闲");
			}
	
		}		
	}

	closesocket(servSock);
	closesocket(localSock);
	WSACleanup();				//释放ws2_32.dll动态链接库初始化时分配的资源

	system("pause");
	return 0;
}

