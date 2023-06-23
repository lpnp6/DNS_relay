//#include<winsock2.h>
//#include<windows.h>
//#pragma comment (lib,"Ws2_32.lib")
#include <stdio.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct translate {
    char* IP;        // IP地址
    char* domain;    // 域名
} Translate;

// 从文件中读取数据并存储到Translate数组中
void read(Translate* translateArray, int size, const char* filename) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("Failed to open file.\n");
        return;
    }

    char line[256];
    int count = 0;

    while (count < size && fgets(line, sizeof(line), file) != NULL) {
        char* ip = strtok(line, " \t\n");
        char* domain = strtok(NULL, " \t\n");

        // 分配内存并存储IP和域名
        translateArray[count].IP = strdup(ip);
        translateArray[count].domain = strdup(domain);

        count++;
    }

    fclose(file);
}
const int size = 1000;
// 查找域名对应的IP地址
const char* findIP(const Translate* translateArray, int size, const char* domain) {
    for (int i = 0; i < size; i++) {
        if (strcmp(translateArray[i].domain, domain) == 0) {
            return translateArray[i].IP;
        }
    }
    return NULL; // 如果未找到匹配的域名，则返回NULL
}

/*int main() {

    Translate* translateArray = (Translate*)malloc(size * sizeof(Translate));

    // 从文件中读取数据并存储到Translate数组中
    read(translateArray, size, "C://Users//32066//Desktop//计算机网络课程设计材料//dnsrelay.txt");

    // 使用translateArray进行操作

    const char* domainToFind = "ad4.sina.com.cn";
    const char* foundIP = findIP(translateArray, size, domainToFind);

    if (foundIP != NULL) {
        printf("IP for domain %s: %s\n", domainToFind, foundIP);
    }
    else {
        printf("No IP found for domain %s\n", domainToFind);
    }

    printf("%s",translateArray[207].domain);
    //// 释放Translate数组中每个实例的内存
    for (int i = 0; i < strlen(translateArray); i++) {
        free(translateArray[i].IP);
        free(translateArray[i].domain);
    }*/

    // 释放Translate数组的内存
    free(translateArray);

    return 0;
}
