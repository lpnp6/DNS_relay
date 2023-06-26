#pragma warning(disable:4996)
#include"Header.h"

std::map<string, string>ip_domain_Map;//DNS域名解析表

void read(const char* filename) {
	FILE* file = fopen(filename, "r");
	if (file == NULL) {
		printf("Failed to open file.\n");
		exit(0);
	}
	printf("Try to load table  % s ... OK\n", filename);
	char line[256];

	int NameCount = 0;
	int ByteCount = 0;

	while (fgets(line, sizeof(line), file) != NULL) {
		const char* ip = strtok(line, " \t\n");
		const char* domain = strtok(NULL, " \t\n");

		if (ip == NULL || domain == NULL) continue;

		ByteCount += sizeof(ip) + sizeof(domain);

		string Domain(domain);
		string Ip(ip);
		
		// 分配内存并存储IP和域名
		ip_domain_Map.insert(std::make_pair(Domain, Ip));
		NameCount++;
	}

	printf("%d names, occupy %d bytes memory\n ", NameCount, ByteCount);

	fclose(file);
}
// 查找域名对应的IP地址

const char* findIP(const char* domain) {
	string Domain(domain);
	string Ip = "";
	auto it = ip_domain_Map.find(Domain);
	if (it != ip_domain_Map.end())
	{
		Ip = it->second;
		char* ptr = (char*)malloc(sizeof(IPLength + 1));
		strcpy(ptr, Ip.c_str());
		return ptr;
	}
	return NULL;
}
