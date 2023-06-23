#pragma once
#define MAX_FILE_LENGTH 250

char filePath[MAX_FILE_LENGTH];
int DebugLevel = 0;


void setParameter(int argc, char* argv[])
{
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
			strcpy(serverIPAddress, argv[1]);
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
			strcpy(serverIPAddress, argv[2]);
		}
		else
		{
			strcpy(serverIPAddress, argv[1]);
			strcpy(filePath, argv[2]);
		}
	}
	else
	{
		if (argv[1][1] == 'd')
			DebugLevel = 1;
		if (argv[1][2] == 'd')
			DebugLevel = 2;
		strcpy(serverIPAddress, argv[2]);
		strcpy(filePath, argv[3]);
	}
	printf("Name server %s:53\n", serverIPAddress);
	printf("Debug level %d\n", DebugLevel);
}
