#pragma warning (disable:4996)
#include"Header.h"

char LocalDNSAddress[IPLength];
char DefDNSAddress[IPLength];
char filePath[MAX_FILE_LENGTH];
int DebugLevel;

void Init()
{
	strcpy(DefDNSAddress, "10.3.9.44");
	strcpy(LocalDNSAddress, "127.0.0.1");
	strcpy(filePath, "dnsrelay.txt");
	DebugLevel = 0;
}

void setParameter(int argc, char* argv[])
{
	Init();
	if (argc == 2)
	{
		if (argv[1][0] == '-')
		{
			if (argv[1][1] == 'd')
				DebugLevel = 1;
			if (argv[1][2] == 'd')
				DebugLevel = 2;
		}
		else
		{
			strcpy(DefDNSAddress, argv[1]);
		}
	}
	else if (argc == 3)
	{
		if (argv[1][0] == '-')
		{
			if (argv[1][1] == 'd')
				DebugLevel = 1;
			if (argv[1][2] == 'd')
				DebugLevel = 2;
			strcpy(DefDNSAddress, argv[2]);
		}
		else
		{
			strcpy(DefDNSAddress, argv[1]);
			strcpy(filePath, argv[2]);
		}
	}
	else if (argc == 4)
	{
		if (argv[1][1] == 'd')
			DebugLevel = 1;
		if (argv[1][2] == 'd')
			DebugLevel = 2;
		strcpy(DefDNSAddress, argv[2]);
		strcpy(filePath, argv[3]);
	}
	printf("Name server %s:53\n", DefDNSAddress);
	printf("Debug level %d\n", DebugLevel);
}
