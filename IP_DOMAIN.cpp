#pragma warning(disable:4996)
#include"Header.h"

map<const char*, const char*>ip_domain_Map;//DNS域名解析表

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

		std::pair<const char*, const char*>Pair;
		Pair.first = domain;
		Pair.second = ip;
		//printf("%s   %s\n",ip,domain);
		// 分配内存并存储IP和域名
		ip_domain_Map.insert(Pair);
		NameCount++;
	}

	printf("%d names, occupy %d bytes memory\n ", NameCount, ByteCount);

	fclose(file);
	//return NameCount;
}
// 查找域名对应的IP地址

const char* findIP(const char* domain) {
	auto it = ip_domain_Map.find(domain);
	if (it != ip_domain_Map.end())
		return it->second;
	return NULL;
}
