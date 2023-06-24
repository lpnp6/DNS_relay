#include"Header.h"

void getURL(char* rev, char* url)
{
	int i = 0, j = 0, k = 0, length = strlen(rev);
	while (i < length)
	{
		if (rev[i] > 0 && rev[i] <= 63)
		{
			for (j = rev[i], i++; j > 0; i++, k++, j--)
				url[k] = rev[i];
			if (rev[i] != 0)
				url[k++] = '.';
		}
	}
	url[k] = 0;
}
