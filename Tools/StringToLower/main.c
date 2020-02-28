#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
//-----------------------------------------------------------------------------
int main(int argc, char **argv)
{
	bool Result = argc == 2;
	if (Result)
	{
		char *String = argv[1];
		for (size_t i = 0, c = strlen(String); i < c; ++i)
		{
			String[i] = (char)tolower(String[i]);
		}
		printf("%s", String);
	}
	else
	{
		printf("Error: Not specified string.\n");
		printf("Using: StringToLower");
#ifdef WIN32
		printf(".exe");
#endif
		printf(" [STRING]\n");
	}
	return Result ? EXIT_SUCCESS : EXIT_FAILURE;
}
//-----------------------------------------------------------------------------
