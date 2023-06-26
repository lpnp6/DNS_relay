#pragma warning (disable:4996)
#include"Header.h"

extern ID_trans id_trans_table[AMOUNT];	//ID转换表
extern int IDcount;			//转换表中的条目个数
extern char Url[lengthURL];					//域名
extern SYSTEMTIME TimeOfSys;                     //系统时间
extern int Day, Hour, Minute, Second, Milliseconds;//保存系统时间的变量
extern map<string,string>ip_domain_Map;//DNS域名解析表
extern char LocalDNSAddress[IPLength];
extern char DefDNSAddress[IPLength];
extern char filePath[MAX_FILE_LENGTH];
extern int DebugLevel;

int main(int argc,char** argv)
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
	char recvBuf[BUFSIZE]; //接收缓存

	int client_len, iSend, iRecv;

	read(filePath);
	//保存系统时间
	GetLocalTime(&TimeOfSys);
	Day = TimeOfSys.wDay;
	Hour = TimeOfSys.wHour;
	Minute = TimeOfSys.wMinute;
	Milliseconds = TimeOfSys.wMilliseconds;

	const char* getIP;
	unsigned short NewID;
	unsigned short* pID;

	//下面是服务器的具体操作
	while (1)
	{
		client_len = sizeof(clientName);
		memset(recvBuf, 0, BUFSIZE); //将接收缓存先置为全0

		//接收DNS请求
		//函数：int recvfrom(int s, void* buf, int len, unsigned int flags, struct sockaddr* from, int* fromlen);
		//函数说明：recv()用来接收远程主机经指定的socket 传来的数据, 并把数据存到由参数buf 指向的内存空间, 参数len 为可接收数据的最大长度.
		//参数flags 一般设0, 其他数值定义请参考recv().参数from 用来指定欲传送的网络地址, 结构sockaddr 请参考bind().参数fromlen 为sockaddr 的结构长度.
		iRecv = recvfrom(localSock, recvBuf, sizeof(recvBuf), 0, (SOCKADDR*)&clientName, &client_len);
		//错误反馈
		if (iRecv == SOCKET_ERROR)
		{
			//printf("Recvfrom Failed: %s\n", strerror(WSAGetLastError()));
			continue; //强制开始下一次循环
		}
		else if (iRecv == 0)
		{
			break; //没东西，跳出循环0
		}
		else
		{
			DNS_Packet packet;
			parseDNSPacket(&packet, recvBuf);
			//getURL(recvBuf, Url);//获取域名
			GetUrl(recvBuf, iRecv);
			getIP = findIP(Url);	//在域名解析表中查找

			pID = (unsigned short*)malloc(sizeof(unsigned short*));
			memcpy(pID, recvBuf, sizeof(unsigned short)); //报文前两字节为ID
			if (DebugLevel >= 1)
				PrintRecvInfo(clientName, &packet, ntohs(*pID), Url, iRecv);
			//printf("We have get the url: %s\n", Url);

			//printf("%d\n", find);

			//开始分情况讨论
			//在域名解析表中没有找到
			if (getIP == NULL)
			{
				
				//printf("We dont find this url, will get a new ID and forward to SERVER.\n");
				//ID转换
				//pID = new (unsigned short);
				
				NewID = htons(replace_id(ntohs(*pID), clientName, FALSE));
				memcpy(recvBuf, &NewID, sizeof(unsigned short));

				//把recvbuf转发至指定的外部DNS服务器
				iSend = sendto(servSock, recvBuf, iRecv, 0, (SOCKADDR*)&serverName, sizeof(serverName));
				if (iSend == SOCKET_ERROR)
				{
					//printf("sendto Failed: %s\n", strerror(WSAGetLastError()));
					continue;
				}
				else if (iSend == 0)
					break;
				else if (iSend > 0 && DebugLevel >= 1)
					PrintSendInfo(serverName, &packet, *pID, NewID, getIP);

				//delete pID; //释放动态分配的内存
				free(pID);
				clock_t start, stop; //定时
				double duration = 0;

				//接收来自外部DNS服务器的响应报文
				start = clock();
				iRecv = recvfrom(servSock, recvBuf, sizeof(recvBuf), 0, (SOCKADDR*)&clientName, &client_len);
				if (iRecv > 0 && DebugLevel >= 1)
					PrintRecvInfo(clientName, &packet, ntohs(*(u_short*)recvBuf), Url, iRecv);
				while ((iRecv == 0) || (iRecv == SOCKET_ERROR))
				{
					iRecv = recvfrom(servSock, recvBuf, sizeof(recvBuf), 0, (SOCKADDR*)&clientName, &client_len);
					if (iRecv > 0 && DebugLevel >= 1)
						PrintRecvInfo(clientName, &packet, ntohs(*(u_short*)recvBuf), Url, iRecv);
					stop = clock();
					duration = (double)(stop - start) / CLK_TCK;
					if (duration > 5)
					{
						printf("Long Time No Response From Server.\n");
						continue;
					}
				}
				//ID转换
				pID = (unsigned short*)malloc(sizeof(unsigned short*));
				memcpy(pID, recvBuf, sizeof(unsigned short)); //报文前两字节为ID
				int GetId = ntohs(*pID); //ntohs的功能：将网络字节序转换为主机字节序
				unsigned short oID = htons(id_trans_table[GetId].oid);
				memcpy(recvBuf, &oID, sizeof(unsigned short));
				id_trans_table[GetId].done = TRUE;

				//char* urlname;
				//memcpy(urlname, &(recvBuf[sizeof(DNSHDR)]), iRecv - 12);	//获取请求报文中的域名表示，要去掉DNS报文首部的12字节
				//char* NewIP;

				//打印 时间 newID 功能 域名 IP
				//PrintInfo(ntohs(NewID), getIP);

				//从ID转换表中获取发出DNS请求者的信息
				clientName = id_trans_table[GetId].client;

				//printf("We get a answer from SERVER, now we give it back to client.\n");

				//把recvbuf转发至请求者处
				iSend = sendto(localSock, recvBuf, iRecv, 0, (SOCKADDR*)&clientName, sizeof(clientName));
				if (iSend == SOCKET_ERROR)
				{
					//printf("sendto Failed: %s\n\n", strerror(WSAGetLastError()));
					continue;
				}
				else if (iSend == 0)
					break;
				else if (iSend > 0 && DebugLevel > 0)
					PrintSendInfo(clientName,&packet, *pID, oID, getIP);
				free(pID); //释放动态分配的内存
			}

			//在域名解析表中找到
			else
			{
				//printf("We have find this url.\n");
				//获取请求报文的ID
				unsigned short nID = replace_id(ntohs(*pID), clientName, FALSE);

				//printf("We have get a new ID, now we will create an answer.\n");

				//打印 时间 newID 功能 域名 IP
				//if (DebugLevel >= 1)
					//PrintRecvInfo(clientName,packet,ntohs(pID),ntohl(NewID),Url);

				//构造响应报文头
				memcpy(sendBuf, recvBuf, iRecv); //拷贝请求报文
				unsigned short AFlag = htons(0x8180); //htons的功能：将主机字节序转换为网络字节序，即大端模式(big-endian) 0x8180为DNS响应报文的标志Flags字段
				memcpy(&sendBuf[2], &AFlag, sizeof(unsigned short)); //修改标志域,绕开ID的两字节

				//修改回答数域

				if (strcmp(getIP, "0.0.0.0") == 0)
					AFlag = htons(0x0000);	//屏蔽功能：回答数为0
				else
					AFlag = htons(0x0001);	//服务器功能：回答数为1
				memcpy(&sendBuf[6], &AFlag, sizeof(unsigned short)); //修改回答记录数，绕开ID两字节、Flags两字节、问题记录数两字节

				int curLen = 0; //不断更新的长度

				//构造DNS响应部分
				char answer[16];
				unsigned short Name = htons(0xc00c); //域名指针（偏移量）
				memcpy(answer, &Name, sizeof(unsigned short));
				curLen += sizeof(unsigned short);

				unsigned short TypeA = htons(0x0001); //类型
				memcpy(answer + curLen, &TypeA, sizeof(unsigned short));
				curLen += sizeof(unsigned short);

				unsigned short ClassA = htons(0x0001); //查询类
				memcpy(answer + curLen, &ClassA, sizeof(unsigned short));
				curLen += sizeof(unsigned short);

				//TTL四字节
				unsigned long timeLive = htonl(0x7b); //生存时间
				memcpy(answer + curLen, &timeLive, sizeof(unsigned long));
				curLen += sizeof(unsigned long);

				unsigned short RDLength = htons(0x0004); //资源数据长度
				memcpy(answer + curLen, &RDLength, sizeof(unsigned short));
				curLen += sizeof(unsigned short);

				unsigned long IP = (unsigned long)inet_addr(getIP); //inet_addr为IP地址转化函数
				memcpy(answer + curLen, &IP, sizeof(unsigned long));
				curLen += sizeof(unsigned long);
				curLen += iRecv;


				//请求报文和响应部分共同组成DNS响应报文存入sendbuf
				memcpy(sendBuf + iRecv, answer, curLen);


				iSend = sendto(localSock, sendBuf, curLen, 0, (SOCKADDR*)&clientName, sizeof(clientName));
				if (iSend > 0)
					PrintSendInfo(clientName, &packet, *pID, nID, getIP);


				free(pID); //释放动态分配的内存

				//printf("\nThis loop is over, thanks.\n\n");
			}
		}
	}

	closesocket(servSock);
	closesocket(localSock);
	WSACleanup();				//释放ws2_32.dll动态链接库初始化时分配的资源

	system("pause");
	return 0;
}
