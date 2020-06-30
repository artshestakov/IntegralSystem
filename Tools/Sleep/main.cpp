#include <iostream>
#include <cstdlib>
#include <climits>
//-----------------------------------------------------------------------------
#ifdef WIN32
#include <windows.h>
#define SLEEP(x) ::Sleep(x * 1000)
#else
#include <unistd.h>
#define SLEEP(x) sleep(x)
#endif
//-----------------------------------------------------------------------------
int main(int argc, char **argv)
{
	bool Result = argc > 1;
	if (Result)
	{
		unsigned long Seconds = std::atol(argv[1]);
		if (Seconds > 0 && Seconds + 1 != ULONG_MAX)
		{
            SLEEP(Seconds);
		}
		else
		{
			std::cout << "Error: invalid argument: " << argv[1] << std::endl;
		}
	}
	else
	{
		std::cout << "Error: not specified arguments." << std::endl;
		std::cout << "Using: Sleep";
#ifdef WIN32
		std::cout << ".exe";
#endif
		std::cout << " [SECONDS]" << std::endl;
		std::cout << "Arguments:" << std::endl;
		std::cout << "SECONDS - count of seconds" << std::endl;
	}
	return Result ? EXIT_SUCCESS : EXIT_FAILURE;
}
//-----------------------------------------------------------------------------
