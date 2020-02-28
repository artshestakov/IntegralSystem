#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
//-----------------------------------------------------------------------------
#ifdef WIN32
#include <windows.h>
#endif
//-----------------------------------------------------------------------------
int main(int argc, char ** argv)
{
	bool Result = argc > 1;
	if (Result)
	{
		char *Argument = argv[1];
		if (strcmp(Argument, "--getdate") == 0)
		{
			SYSTEMTIME ST;
			GetLocalTime(&ST);
			printf("%02d.%02d.%02d", ST.wDay, ST.wMonth, ST.wYear);
		}
		else if (strcmp(Argument, "--gettime") == 0)
		{
			SYSTEMTIME ST;
			GetLocalTime(&ST);
			printf("%02d:%02d", ST.wHour, ST.wMinute);
		}
		else
		{
			printf("Error: invalid argument\n");
			Result = false;
		}
	}
	else
	{
		printf("Error: not specified arguments.\n");
		printf("Using: GET");
#ifdef WIN32
		printf(".exe");
#endif
		printf(" [ARGUMENT]\n");
		printf("Arguments:\n");
		printf("--getdate - get current date\n");
		printf("--gettime - get current time\n");
	}
	return Result ? EXIT_SUCCESS : EXIT_FAILURE;
}
//-----------------------------------------------------------------------------
