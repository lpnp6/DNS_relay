#pragma warning(disable:4996)
#include"Header.h"

std::map<const char*, const char*>Map;

void read(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("Failed to open file.\n");
        exit(0);
    }
    else
        printf("Try to load table  % s ... OK\n", filename);
    char line[256];

    int NameCount = 0;
    int ByteCount = 0;

    while (fgets(line, sizeof(line), file) != NULL) {
        const char* ip = strtok(line, " \t\n");
        const char* domain = strtok(NULL, " \t\n");

        ByteCount += sizeof(ip) + sizeof(domain);

        std::pair<const char*, const char*>Pair;
        Pair.first = domain;
        Pair.second = ip;

        // 分配内存并存储IP和域名
        Map.insert(Pair);
        NameCount++;
    }

    printf("%d names, occupy %d bytes memory\n ", NameCount, ByteCount);

    fclose(file);
}
// 查找域名对应的IP地址

const char* findIP(const char* domain) {
    auto it = Map.find(domain);
    if (it != Map.end())
        return it->second;
    else
        return NULL;
}
