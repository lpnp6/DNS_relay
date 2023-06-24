#include"Header.h"

int main()
{
    char sendBuf[BUFSIZE]; //发送缓存
    char recvBuf[BUFSIZE]; //接收缓存
    char* Path;//本地TXT路径
    Path = (char*)malloc(sizeof(char) * 100);
    int recordNum= read(Path);; //txt文件有效行数
    int iLen_cli, iSend, iRecv;//socket传的数据长度
    strcpy(Path, "C:\\Users\\mrw29\\Desktop\\dnsrelay.txt");//
    Day = TimeOfSys.wDay;
    Hour = TimeOfSys.wHour;
    Minute = TimeOfSys.wMinute;
    Milliseconds = TimeOfSys.wMilliseconds;
    int find;//在域名解析表中找没找到
    unsigned short NewID;
    unsigned short* pID;
    while (1)
    {
        clientName_len = sizeof(clientName);//请求端的网络套接字地址
        memset(recvBuf, 0, BUFSIZE);
        iRecv = recvfrom(localSock, recvBuf, sizeof(recvBuf), 0, (SOCKADDR*)&clientName, &clientName_len);
        //用来接收远程主机经指定的socket 传来的数据, 并把数据存到由参数recvbuf 指向的内存空间, 参数len 为可接收数据的最大长度
        if (iRecv == SOCKET_ERROR)
            	{
            			printf("Recvfrom Failed: %s\n", strerror(WSAGetLastError()));
            			continue; //强制开始下一次循环
            	}
        else if (iRecv == 0)
        {
            break;//没东西
        }
        else
        {
            getURl(recvBuf, iRecv);
            find = findIP(url);//??????函数怎么调用，这里是在域名解析表里查找读到域名的IP地址
            printf("We have get the url: %s\n", url);
            printf("%d\n", find);
            if (find == NOTFOUND)
            {
                printf("We dont find this url, will get a new ID and forward to SERVER.\n");
                //ID转换
                pID = (unsigned short*)malloc(sizeof(unsigned short*));
                memcpy(pID, recvBuf, sizeof(unsigned short));//前两字节为ID
                NewID = htons(ReplaceNewID(ntohs(*pID), clientName, FALSE));//htons和ntohs函数转换字节顺序
                memcpy(recvBuf, &NewID, sizeof(unsigned short));//复制新ID
                //打印 时间 newID 功能 域名 IP
                PrintInfo(ntohs(NewID), find);
                //把要查询的域名报文发送到外部的DNS服务器
                iSend = sendto(servSock, recvBuf, iRecv, 0, (SOCKADDR*)&serverName, sizeof(serverName));
                if (iSend == SOCKET_ERROR)
                {
                    printf("sendto Failed: %s\n", strerror(WSAGetLastError()));
                    continue;
                }
                else if (iSend == 0) break;
                free(pID);//释放内存
                clock_t start, stop; //定时
                double duration = 0;
                //接收来自外部DNS服务器的响应报文
                start = clock();
                iRecv = recvfrom(servSock, recvBuf, sizeof(recvBuf), 0, (SOCKADDR*)&clientName, &clientName_len);
                while ((iRecv == 0) || (iRecv == SOCKET_ERROR))
                {
                    iRecv = recvfrom(servSock, recvBuf, sizeof(recvBuf), 0, (SOCKADDR*)&clientName, &iLen_cli);
                    stop = clock();
                    duration = (double)(stop - start) / CLK_TCK;
                    if (duration > 5)
                    {
                        printf("Long Time No Response From Server.\n");//没有响应报文
                        continue;
                    }
                }
                //ID转换
                pID = (unsigned short*)malloc(sizeof(unsigned short*));
                memcpy(pID, recvBuf, sizeof(unsigned short)); //报文前两字节为ID
                int GetId = ntohs(*pID); //ntohs的功能：将网络字节序转换为主机字节序
                unsigned short oID = htons(IDTransTable[GetId].oldID);
                memcpy(recvBuf, &oID, sizeof(unsigned short));
                IDTransTable[GetId].done = TRUE;
                char* urlname;
                memcpy(urlname, &(recvBuf[sizeof(dns_header)]), iRecv - 12);	//获取请求报文中的域名表示，要去掉DNS报文首部的12字节
                char* NewIP;
                //打印 时间 newID 功能 域名 IP
                PrintInfo(ntohs(NewID), find);
                //从ID转换表中获取发出DNS请求者的信息
                clientName = IDTransTable[GetId].client;
                printf("We get a answer from SERVER, now we give it back to client.\n");
                //把recvbuf转发至请求者处
                iSend = sendto(localSock, recvBuf, iRecv, 0, (SOCKADDR*)&clientName, sizeof(clientName));
                if (iSend == SOCKET_ERROR)
                {
                    printf("sendto Failed: %s\n\n", strerror(WSAGetLastError()));
                    continue;
                }
                else if (iSend == 0)break;
                free(pID);
            }
            else
            {
                //获取请求的ID
                unsigned short* now_id = (unsigned short*)malloc(sizeof(unsigned short*));
                strncpy(now_id, recvBuf, sizeof(unsigned short));
                //ID转换
                unsigned short new_id = trans_NewID(ntohs(*now_id), clientName, FALSE);
                //打印newID，构造响应报文头
                PrintInfo(new_id, find);

                memcpy(sendBuf, recvBuf, iRecv); //复制一份原报文
                unsigned short Flags = htons(0x8180); //新的标志字段
                memcpy(&sendBuf[2], &Flags, sizeof(unsigned short)); //修改标志域,绕开ID的两字节

                    //修改回答数域
                if (strcmp(DNS_domain_Table[find].IP, "0.0.0.0") == 0)
                    Flags = htons(0x0000);	//屏蔽功能：回答数为0
                else
                    Flags = htons(0x0001);	//服务器功能：回答数为1
                memcpy(&sendBuf[6], &Flags, sizeof(unsigned short)); //修改回答记录数，绕开ID两字节、Flags两字节、问题记录数两字节

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

                unsigned long IP = (unsigned long)inet_addr(DNS_domain_Table[find].IP); //inet_addr为IP地址转化函数
                memcpy(answer + curLen, &IP, sizeof(unsigned long));
                curLen += sizeof(unsigned long);
                curLen += iRecv;



                memcpy(sendBuf + iRecv, answer, curLen);

                //发送DNS响应报文

                clock_t Nstart, Nstop; //clock_t为clock()函数返回的变量类型
                double Nduration;

                Nstart = clock();
                iSend = sendto(localSock, sendBuf, curLen, 0, (SOCKADDR*)&clientName, sizeof(clientName));
                if (iSend == SOCKET_ERROR)
                {
                    //printf("sendto Failed: %s\n", strerror(WSAGetLastError()));
                    Nstop = clock();
                    Nduration = (double)(Nstop - Nstart) / CLK_TCK;
                    if (Nduration > 1)
                        printf("nduration>1");
                    continue;
                }
                else if (iSend == 0)
                    break;
                free(now_id);
            }
        }
    }
    return 0;
}
