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

#define A 1
#define NS 2
#define CNAME 5//以上三个表示RR，查询类型，A表示询问IP地址，NS表示要获取该域名的权威名称服务器名称，CNAME暂不知

//DNS报文首部 12字节
typedef struct DNSheader
{
	UINT16 id;		  //16位,该值由发出DNS请求的程序生成，DNS服务器在响应时会使用该ID，这样便于请求程序区分不同的DNS响应。

	//位域排序类似于小段方式
	unsigned char rd : 1;     //1位，期望使用递归解析 (Recursion desired),如果该位为0，表示使用迭代查询方式
	unsigned char tc : 1;     //1位，被截断的报文(Truncated):当响应的总长度超512字节时，只返回前512个字节
	unsigned char aa : 1;     //1位，权威答案(Authoritative answer),表示响应的服务器是否是权威DNS服务器。只在响应消息中有效。
	unsigned char opCode : 4; //4位，0:标准查询 ,指示请求的类型，有请求发起者设定，响应消息中复用该值。 1:反向查询 , 2:服务器状态请求
	unsigned char qr : 1;     //1位，0表示查询请求报文，1表示响应报文
	
	unsigned char rCode : 4;  //4位，响应码(Response coded)，仅用于响应报文
	unsigned char z : 3;      //3位，必须为0，保留字段
	unsigned char ra : 1;     //1位，递归可用(Recursion Available),该值在响应消息中被设置或被清除，以表明服务器是否支持递归查询。

	UINT16 qdCount;   //question section的问题个数，16位
	UINT16 anCount;   //answer section的RR(Resource Record)个数，16位
	UINT16 nsCount;   //authority records section的RR个数，16位
	UINT16 arCount;   //additional records section的RR个数，16位
}DNSHDR, * pDNSHDR;

typedef struct
{
	char* qName;	//字节数不定，以0x00作为结束符。表示查询的主机名。主机名被"."号分割成了多段标签。在QNAME中，每段标签前面加一个数字，表示接下来标签的长度。
	UINT16 qType;	//占2个字节。表示RR类型
	UINT16 qClass;	//占2个字节。表示RR分类
}QUESTION;

//Answer、Authority、Additional部分格式一致，每部分都由若干实体组成，每个实体即为一条RR
typedef struct
{
	char* name;     //域名
	UINT16 type;	//占2个字节。表示RR的类型，如A、CNAME、NS等
	UINT16 rclass;	//占2个字节。表示RR的分类
	UINT32 ttl;		//占4个字节。表示RR生命周期，即RR缓存时长，单位是秒
	UINT16 rdLength;//占2个字节。指定RDATA字段的字节数
	char* rData;    //资源数据
}RR;

typedef struct DnsPacket
{
	DNSHeader* header;

	QUESTION* queries;//查询问题区域，动态分配的数组个数由qdCount决定
	RR* answers;	//回答问题区域，动态分配的数组个数由anCount决定
	RR* authority;	//权威名称服务器区域，动态分配的数组个数由nsCount决定
	RR* additional;	//附加信息区域,动态分配的数组个数由arCount决定

}DNS_Packet;

//ID转换表结构
typedef struct ID_Change
{
	unsigned short oid;			//原有ID
	BOOL done;						//标记是否完成解析
	SOCKADDR_IN client;				//请求者套接字地址
} ID_trans;

//从上层DNS获取的资源链表结点
typedef struct Cached
{
	int ttl;
	uint32_t ipAddress;//IP地址
	char* domainName;//域名
	char* cName;//别名
	struct Cached* nextCachedPtr;
}CACHED;
typedef CACHED* CACHED_PTR;//后续有机会再用

#endif 

#pragma  comment(lib, "Ws2_32.lib") //加载 ws2_32.dll

void read(const char* filename);// 查找域名对应的IP地址
const char* findIP(const char* domain); //获取DNS请求中的域名

void GetUrl(char* recvbuf, int recvnum);

unsigned short replace_id(unsigned short OldID, SOCKADDR_IN temp, BOOL ifdone);

void PrintInfo(unsigned short newID, const char* getIP);

void setParameter(int argc,char* argv[]); DNSHDR, * pDNSHDR;


#endif 

#pragma  comment(lib, "Ws2_32.lib") //加载 ws2_32.dll

void read(const char* filename);// 查找域名对应的IP地址
const char* findIP(const char* domain); //获取DNS请求中的域名

void GetUrl(char* recvbuf, int recvnum);

unsigned short replace_id(unsigned short OldID, SOCKADDR_IN temp, BOOL ifdone);

void PrintInfo(unsigned short newID, const char* getIP);

void setParameter(int argc,char* argv[]);
