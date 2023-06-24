#include <utility>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <WinSock2.h>
#include <windows.h>
#include <time.h>
using namespace std;
#include <map>

#ifndef DEFINITION_H_INCLUDED
#define DEFINITION_H_INCLUDED

#pragma once

#define BUFSIZE 1024 //最大报文缓存大小
#define PORT 53   //53端口号
#define AMOUNT 1500//最大ID转换表大小
#define NOTFOUND 32767 //没有找到
#define  LOCAL_DNS_ADDRESS "127.0.0.1"
#define DEF_DNS_ADDRESS "10.3.9.45"
#define lengthURL  64 //0~63
#define IPLength 16
#define MAX_FILE_LENGTH 253

//DNS报文首部 12字节
typedef struct DNSHeader
{
	unsigned short ID; //标志
	unsigned short Flags; //标识
	unsigned short QuestionNum;  //问题数
	unsigned short AnswerNum; //资源记录数
	unsigned short AuthorNum; //授权资源记录数
	unsigned short AdditionNum; //额外资源记录数
} DNSHDR, * pDNSHDR;


//ID转换表结构
typedef struct ID_Change
{
	unsigned short oid;			//原有ID
	BOOL done;						//标记是否完成解析
	SOCKADDR_IN client;				//请求者套接字地址
} ID_trans;

#endif 

#pragma  comment(lib, "Ws2_32.lib") //加载 ws2_32.dll

void read(const char* filename);// 查找域名对应的IP地址
const char* findIP(const char* domain); //获取DNS请求中的域名

void GetUrl(char* recvbuf, int recvnum);

unsigned short replace_id(unsigned short OldID, SOCKADDR_IN temp, BOOL ifdone);

void PrintInfo(unsigned short newID, const char* getIP);

void setParameter(int argc,char* argv[]);
