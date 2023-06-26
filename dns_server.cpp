#include"Header.h"
#pragma warning(disable:4996)

ID_trans id_trans_table[AMOUNT];	//ID转换表
int IDcount = 0;					//转换表中的条目个数
char Url[lengthURL];					//域名
SYSTEMTIME TimeOfSys;                     //系统时间
int Day, Hour, Minute, Second, Milliseconds;//保存系统时间的变量

extern int DebugLevel;

void GetUrl(char* recvbuf, int recvnum)
{
	char urlname[lengthURL];
	int i = 0, j, k = 0;

	memset(Url, 0, lengthURL); //全用0初始化
	memcpy(urlname, &(recvbuf[sizeof(DNSHDR)]), recvnum - 12);	//获取请求报文中的域名表示，要去掉DNS报文首部的12字节

	int len = strlen(urlname);

	//域名转换
	while (i < len)
	{
		if (urlname[i] > 0 && urlname[i] <= 63)
			for (j = urlname[i], i++; j > 0; j--, i++, k++)
				Url[k] = urlname[i];

		if (urlname[i] != 0)
			Url[k++] = '.';
	}

	Url[k] = '\0';
}

unsigned short replace_id(unsigned short OldID, SOCKADDR_IN temp, BOOL ifdone)
{
	id_trans_table[IDcount].oid = OldID;
	id_trans_table[IDcount].client = temp;
	id_trans_table[IDcount].done = ifdone;
	IDcount++; //ID转换表数目要更新

	return (unsigned short)(IDcount - 1);	//以表中下标作为新的ID
}

void PrintRecvInfo(sockaddr_in clientname, DnsPacket* packet, unsigned short ID, const char* url, int iecv)
{
	//打印时间
	GetLocalTime(&TimeOfSys);
	//输出指定长度的字符串, 超长时不截断, 不足时左对齐:
	//printf("%-ns", str);            --n 为指定长度的10进制数值
	int Btime;
	int Ltime;
	Btime = ((((TimeOfSys.wDay - Day) * 24 + TimeOfSys.wHour - Hour) * 60 + TimeOfSys.wMinute - Minute) * 60) + TimeOfSys.wSecond - Second;
	Ltime = abs(TimeOfSys.wMilliseconds - Milliseconds);
	printf("%d.%d   %d", Btime, Ltime, ID);
	printf("    ");
	printf("receive from %s:%d", inet_ntoa(clientname.sin_addr), ntohs(clientname.sin_port));
	printf("    ");
	printf("%s\n", url);
	if (DebugLevel >= 2)
	{
		printf("(%d bytes)", iecv);
		printHex((char*)packet, iecv);
		printf("ID %04X, QR %d, OPCODE %d, AA %d, TC %d, RD %d, RA %d, Z %d, RCODE %d\n QDCOUNT %hu, ANCOUNT %hu, NSCOUNT %hu, ARCOUNT %hu\n ", packet->header->id, packet->header->qr, packet->header->opCode, packet->header->aa, packet->header->tc, packet->header->rd, packet->header->ra, packet->header->z,
			packet->header->rCode, packet->header->qdCount, packet->header->anCount, packet->header->nsCount, packet->header->arCount);
		if (clientname.sin_port == 53)
			printf("TYPE %hu CLASS %hu", packet->answers->type, packet->answers->rclass);
		else
			printf("TYPE %hu CLASS %hu", packet->queries->qType, packet->queries->qClass);
	}
	printf("\n");
	printf("\n");
}

void PrintSendInfo(sockaddr_in clientname, DnsPacket* packet, unsigned short oldID, unsigned short newID, const char* getIP)
{
	//打印时间
	GetLocalTime(&TimeOfSys);
	//输出指定长度的字符串, 超长时不截断, 不足时左对齐:
	//printf("%-ns", str);            --n 为指定长度的10进制数值
	int Btime;
	int Ltime;
	Btime = ((((TimeOfSys.wDay - Day) * 24 + TimeOfSys.wHour - Hour) * 60 + TimeOfSys.wMinute - Minute) * 60) + TimeOfSys.wSecond - Second;
	Ltime = abs(TimeOfSys.wMilliseconds - Milliseconds);
	printf("%d.%d   %d", Btime, Ltime, newID);
	printf("    ");

	//在表中没有找到DNS请求中的域名
	if (getIP == NULL)
	{
		//中继功能
		printf("Send to %s:53", DEF_DNS_ADDRESS);

		printf("    ");
		//打印域名
		if (DebugLevel >= 2)
		{
			printf("[ID %02X -> %02X]", oldID, newID);
			printf("    ");
		}
		printf("%s", Url);
		printf("    ");
		//打印IP
		printf("\n");
	}

	//在表中找到DNS请求中的域名
	else
	{
		if (strcmp(getIP, "0.0.0.0") == 0)  //不良网站拦截
		{
			//屏蔽功能
			printf("blocked");
			printf("    ");
			//打印域名(加*)
			//打印域名
			printf("%s", Url);
			printf("    ");
			//打印IP
			printf("%s\n", getIP);
		}

		//检索结果为普通IP地址，则向客户返回这个地址
		else
		{
			//服务器功能
			printf("Send to %s:%d", LOCAL_DNS_ADDRESS, clientname.sin_port);
			printf("    ");
			//打印域名
			if (DebugLevel >= 2)
			{
				printf("[ID %02X -> %02X]", oldID, newID);
				printf("    ");
			}
			printf("%s", Url);
			printf("    ");
			//打印IP
			printf("%s\n", getIP);
		}
	}
	printf("\n");
}
