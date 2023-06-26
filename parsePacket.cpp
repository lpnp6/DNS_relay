#include "Header.h"

void printBinary(const char* data, int a) {
	// 遍历字符数组，直到遇到空字符 '\0'
	for (int i = 0; i < a; i++) {
		unsigned char c = (unsigned char)data[i];
		// 逐位输出每个字符的二进制表示
		for (int j = 7; j >= 0; j--) {
			unsigned char bit = (c >> j) & 1;
			printf("%u", bit);
		}
		printf(" ");
	}
	printf("\n");
}

void printHex(const char* data, size_t length) {
	for (size_t i = 0; i < length; i++) {
		printf("%02X ", (unsigned char)data[i]);
	}
	printf("\n");
}

// 解析DNS报头并将解析结果存储在DNSHDR结构中
void parseDNSHeader(DNSHDR* dnsHeader, char* recvBuf) {
	// 解析ID字段

	dnsHeader->id = ntohs(*(UINT16*)(recvBuf));

	// 解析标志字段

	UINT16 flags = ntohs(*(UINT16*)(recvBuf + 2));
	dnsHeader->rd = (flags >> 15) & 0x01;
	dnsHeader->tc = (flags >> 14) & 0x01;
	dnsHeader->aa = (flags >> 13) & 0x01;
	dnsHeader->opCode = (flags >> 9) & 0x0F;
	dnsHeader->qr = (flags >> 8) & 0x01;
	dnsHeader->rCode = (flags >> 4) & 0x0F;
	dnsHeader->z = (flags >> 1) & 0x07;
	dnsHeader->ra = flags & 0x01;


	// 解析问题计数字段
	dnsHeader->qdCount = ntohs(*(UINT16*)(recvBuf + 4));


	// 解析回答计数字段
	dnsHeader->anCount = ntohs(*(UINT16*)(recvBuf + 6));

	// 解析授权记录计数字段
	dnsHeader->nsCount = ntohs(*(UINT16*)(recvBuf + 8));

	// 解析附加记录计数字段
	dnsHeader->arCount = ntohs(*(UINT16*)(recvBuf + 10));

}



// 解析问题字段
void parseQuestion(QUESTION* question, UINT16 count, char* recvBuf, int* offset) {

	for (int i = 0; i < count; i++) {
		int qNameLength = 0;
		int currentPosition = *offset;

		// 计算 QNAME 的长度
		while (recvBuf[currentPosition] != 0) {
			int labelLength = recvBuf[currentPosition];
			currentPosition += labelLength + 1;
			qNameLength += labelLength + 1;
		}
		qNameLength++; // 加上末尾的 0 字节

		// 分配内存并复制 QNAME
		question->qName = (char*)malloc(qNameLength * sizeof(char));
		int qNameIndex = 0;
		currentPosition = *offset;
		while (recvBuf[currentPosition] != 0) {
			int labelLength = recvBuf[currentPosition];
			for (int i = 0; i < labelLength; i++) {
				question->qName[qNameIndex++] = recvBuf[currentPosition + i + 1];
			}
			question->qName[qNameIndex++] = '.';
			currentPosition += labelLength + 1;
		}
		question->qName[qNameIndex - 1] = '\0'; // 移除末尾多余的 '.'

		*offset += qNameLength;

		// 解析 QTYPE 和 QCLASS
		question->qType = recvBuf[*offset] << 8 | recvBuf[*offset + 1];
		*offset += 2;

		question->qClass = recvBuf[*offset] << 8 | recvBuf[*offset + 1];
		*offset += 2;
	}
}

void parseResourceFields(RR* resources, int count, char* recvBuf, int* offset) {
	//printHex(recvBuf, 200);
	for (int i = 0; i < count; i++) {
		memset(resources + i, 0, sizeof(resources[i]));
		// 解析资源字段的各个字段
		char nameBuffer[512];
		int nameIndex = 0;
		bool isCompressed = false;
		int ss = 0;//判断加.

		int off = *offset;
		while (true) {
			unsigned char labelLength = recvBuf[off++];

			if (labelLength == 0) {
				nameBuffer[nameIndex] = '\0';
				break;
			}

			if ((labelLength & 0xC0) == 0xC0) {
				// 标签长度的最两位为 1，表示使用压缩指针
				unsigned char pointerOffset = recvBuf[off - 1] & 0x3F;
				pointerOffset = (pointerOffset << 8) + recvBuf[off];
				off += sizeof(unsigned char);
				if (!isCompressed) {
					isCompressed = true;
					nameIndex = 0;  // 如果遇到压缩指针，重置域名索引
				}
				off = pointerOffset;
			}
			else {
				// 解析标签值
				if (ss != 0) nameBuffer[nameIndex++] = '.';
				for (int i = 0; i < labelLength; i++) {
					nameBuffer[nameIndex++] = recvBuf[(off)++];
				}
				ss++;

			}
		}
		*offset += sizeof(UINT16);

		// 将域名复制到新的动态分配的缓冲区中

		if (nameIndex > 0) {
			char* qName = (char*)malloc(nameIndex + 1);
			strncpy(qName, nameBuffer, nameIndex + 1);
			qName[nameIndex] = '\0';
			resources[i].name = qName;
			std::cout << "qname  " << qName << endl;
		}
		else
		{
			cout << "qname " << " " << endl;
		}


		resources[i].type = ntohs(*(UINT16*)(recvBuf + *offset));
		//cout << "type;;"<<resources[i].type << endl;
		*offset += sizeof(UINT16);
		resources[i].rclass = ntohs(*(UINT16*)(recvBuf + *offset));
		*offset += sizeof(UINT16);
		resources[i].ttl = ntohl(*(UINT32*)(recvBuf + *offset));
		*offset += sizeof(UINT32);
		resources[i].rdLength = ntohs(*(UINT16*)(recvBuf + *offset));
		//cout << resources[i].rdLength << endl;
		*offset += sizeof(UINT16);
		//cout <<"leng"<< resources[i].rdLength << endl;

		// 解析资源数据
		resources[i].rData = (char*)malloc(resources[i].rdLength);
		memcpy(resources[i].rData, recvBuf + *offset, resources[i].rdLength);


		//printHex(recvBuf,1000);
		*offset += resources[i].rdLength;
	}
}

void parseDNSPacket(DNS_Packet* packet, char* recvBuf) {
	int offset = 0;

	// 解析 DNS 报头
	packet->header = (DNSHDR*)malloc(sizeof(DNSHDR));
	parseDNSHeader(packet->header, recvBuf);
	offset += sizeof(DNSHDR);

	// 解析问题字段
	packet->queries = (QUESTION*)malloc(sizeof(QUESTION) * ntohs(packet->header->qdCount));
	parseQuestion(packet->queries, packet->header->qdCount, recvBuf, &offset);

	// 解析回答字段
	packet->answers = (RR*)malloc(sizeof(RR) * ntohs(packet->header->anCount));
	parseResourceFields(packet->answers, packet->header->anCount, recvBuf, &offset);

	// 解析权威字段
	packet->authority = (RR*)malloc(sizeof(RR) * ntohs(packet->header->nsCount));
	parseResourceFields(packet->authority, packet->header->nsCount, recvBuf, &offset);

	// 解析附加字段
	packet->additional = (RR*)malloc(sizeof(RR) * ntohs(packet->header->arCount));
	parseResourceFields(packet->additional, packet->header->arCount, recvBuf, &offset);
}
