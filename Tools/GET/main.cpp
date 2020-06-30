#include <iostream>
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
#ifdef WIN32
			SYSTEMTIME ST;
			GetLocalTime(&ST);
			printf("%02d.%02d.%02d", ST.wDay, ST.wMonth, ST.wYear);
#endif
		}
		else if (strcmp(Argument, "--gettime") == 0)
		{
#ifdef WIN32
			SYSTEMTIME ST;
			GetLocalTime(&ST);
			printf("%02d:%02d", ST.wHour, ST.wMinute);
#endif
		}
		else
		{
			std::cout << "Error: invalid argument\n" << std::endl;
			Result = false;
		}
	}
	else
	{
		std::cout << "Error: not specified arguments." << std::endl;
		std::cout << "Using: GET" << std::endl;
#ifdef WIN32
		std::cout << ".exe" << std::endl;
#endif
		std::cout << " [ARGUMENT]\n" << std::endl;
		std::cout << "Arguments:\n" << std::endl;
		std::cout << "--getdate - get current date\n" << std::endl;
		std::cout << "--gettime - get current time\n" << std::endl;
	}
	return Result ? EXIT_SUCCESS : EXIT_FAILURE;
}
//-----------------------------------------------------------------------------
