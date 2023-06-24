#pragma once
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <WinSock2.h>
#include <windows.h>
#include <time.h>
#define AMOUNT 1000
#define lengthURL 64 //最大域名长
typedef struct {
    uint16_t ID; // Identification
    uint16_t id;
    uint8_t qr : 1;
    uint8_t opcode : 4;
    uint8_t aa : 1;
    uint8_t tc : 1;
    uint8_t rd : 1;
    uint8_t ra : 1;
    uint8_t z : 3;
    uint8_t rcode : 4;
    uint16_t qdcount;
    uint16_t ancount;
    uint16_t nscount;
} dns_header;//头部

typedef struct {
    uint8_t* qname; // 查询名
    uint16_t qtype; // 查询类型
    uint16_t qclass; // 查询类
    struct dns_query* next;
} dns_query;//查询问题区域

typedef struct {
    char* name; // 域名
    uint16_t type; // 类型
    uint16_t Class; // 类
    uint32_t ttl; // 生存时间
    uint16_t rdlength; // 资源数据长度
    char* rdata; // 资源数据
} dns_record;

typedef struct {
    dns_header* header;
    dns_query* que;
    dns_record* rr;
} dns_packet_t;  //整个报文

typedef struct cache_node {
    dns_packet_t* query; // The query packet
    dns_packet_t* response; // The response packet
    time_t expire_time; // The expire time of the cache node (in seconds since epoch)
    struct cache_node* next; // The pointer to the next cache node in the list
} cache_node_t;

typedef struct translate
{
    char* IP;						//IP地址
    char* domain;					//域名
} ip_domain_trans; //DNS域名解析表

typedef struct ID_change
{
	unsigned short oldID;			//原有ID
	BOOL done;						//标记是否完成解析
	SOCKADDR_IN client;				//请求者套接字地址
} ID_trans;

void getURL(char* rev, char* url);

void setParameter(int argc, char* argv[]);

SOCKET createSocket(short sin_family, u_short sin_port, char* sin_addr);

void read(const char* filename);

const char* findIP(const char* domain);
extern ip_domain_trans DNS_domain_Table[AMOUNT];		//DNS域名解析表
extern ID_trans IDTransTable[AMOUNT];	//ID转换表
int IDcount=0;
int Day, Hour, Minute, Second, Milliseconds;//保存系统时间的变量
char Url[lengthURL];
