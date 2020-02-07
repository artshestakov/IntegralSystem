#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
//-----------------------------------------------------------------------------
void ShowUsing();
//-----------------------------------------------------------------------------
int main(int argc, char **argv)
{
	bool Result = argc == 4 ? true : false;
	if (Result)
	{
		const char *SectionName = argv[1];
		const char *ParameterName = argv[2];
		const char *FilePath = argv[3];

		FILE *File = fopen(FilePath, "r");
		Result = File ? true : false;
		if (Result)
		{
			char Line[1024];
			size_t LineSize = 0;
			size_t Read = 0;
			while (fgets(&Line, sizeof(Line), File) != NULL)
			{
				printf("%s\n", Line);
			}
			fclose(File);
		}
		else
		{
			printf("Error open file %s: %s\n", FilePath, strerror(errno));
		}
	}
	else
	{
		printf("Invalid arguments\n");
		ShowUsing();
	}
	return Result ? EXIT_SUCCESS : EXIT_FAILURE;
}
//-----------------------------------------------------------------------------
void ShowUsing()
{
#ifdef WIN32
	const char Extension[] = ".exe";
	const char FilePath[] = "C:\\server\\config.ini";
#else
	const char Extension[] = "";
	const char FilePath[] = "/tmp/config.ini";
#endif
	printf("Using: INIReader%s <SECTION_NAME> <PARAMETER_NAME> <FILE_PATH.ini>\n", Extension);
	printf("Example: INIReader%s Connection Port %s\n", Extension, FilePath);
}
//-----------------------------------------------------------------------------
