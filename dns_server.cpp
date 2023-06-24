#include"Header.h"

ID_trans id_trans_table[AMOUNT];	//ID转换表
int IDcount = 0;					//转换表中的条目个数
char Url[lengthURL];					//域名
SYSTEMTIME TimeOfSys;                     //系统时间
int Day, Hour, Minute, Second, Milliseconds;//保存系统时间的变量

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
		{
			Url[k] = '.';
			k++;
		}
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

void PrintInfo(unsigned short newID, const char* getIP)
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
		printf("中继");
		printf("    ");
		//打印域名
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
			printf("屏蔽");
			printf("    ");
			//打印域名(加*)
			//打印域名
			printf("***%s", Url);
			printf("    ");
			//打印IP
			printf("%s\n", getIP);
		}

		//检索结果为普通IP地址，则向客户返回这个地址
		else
		{
			//服务器功能
			printf("Local服务器");
			printf("    ");
			//打印域名
			printf("***%s", Url);
			printf("    ");
			//打印IP
			printf("%s\n", getIP);
		}
	}
}
