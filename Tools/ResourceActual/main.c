#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <windows.h>
//-----------------------------------------------------------------------------
const char SOURCE_EXTENSION[] = "*.cpp";
//-----------------------------------------------------------------------------
char *FileContent = NULL;
size_t FileSize = 0;
char **StringList = NULL;
size_t StringListSize = 0;

char **DirsSourceFiles = NULL;
char **FileListSource = NULL; //Список файлов с исходным кодом
size_t FileListSourceSize = 0;
//-----------------------------------------------------------------------------
bool GetSourceFiles();
void ReadResourceFile(const char *FilePath);
void CreateStringList();
char** SearchFiles(const char *DirPath);
//-----------------------------------------------------------------------------
int main(int argc, char **argv)
{
	const char *LangFile = argv[1];
	GetSourceFiles();
	SearchFiles("G:\\Github\\IntegralSystem\\Library\\libCarat\\");

	return 0;




	ReadResourceFile("G:\\Github\\IntegralSystem\\Resources\\Localization\\IntegralSystem.lang");
	CreateStringList();
	

	

	/*for (size_t i = 0; i < LineCount; ++i)
	{
		printf("%s\n", StringList[i]);
	}*/

	return 0;
}
//-----------------------------------------------------------------------------
bool GetSourceFiles()
{
	WIN32_FIND_DATA FindData;
	HANDLE Handle = FindFirstFile(DirPath, &FindData); //Находим первый файл
	if (Handle != INVALID_HANDLE_VALUE) //Первый файл нашли
	{
		do
		{
			if (strcmp(FindData.cFileName, ".") == 0 || strcmp(FindData.cFileName, ".") == 0)
			{
				continue;
			}

			size_t FilePathSize = strlen(DirPath) + strlen(FindData.cFileName) - 5;
			char *FilePath = (char *)malloc(sizeof(char) * FilePathSize + 1);
			if (FilePath)
			{
				memset(FilePath, 0, FilePathSize);
				strncpy(FilePath, DirPath, strlen(DirPath) - 5);
				strcat(FilePath, FindData.cFileName);
			}

			if (!(FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) //Файл
			{
				if (Files)
				{
					Files = (char **)realloc(Files, ++FileCount * sizeof(char *));
				}
				else
				{
					Files = (char **)malloc(sizeof(char *) + ++FileCount);
				}
				Files[FileCount - 1] = FilePath;
			}
		} while (FindNextFile(Handle, &FindData));
		FindClose(Handle);
	}
	return Files;
}
//-----------------------------------------------------------------------------
void ReadResourceFile(const char *FilePath)
{
	FILE *File = fopen(FilePath, "r");
	if (File) //Если файл открылся успешно - читаем его
	{
		fseek(File, 0, SEEK_END);
		FileSize = (size_t)ftell(File);
		rewind(File);

		FileContent = (char *)malloc(FileSize + 1);
		fread(FileContent, sizeof(char), FileSize, File);
		fclose(File);

		//Подсчёт количества строк
		for (size_t i = 0; i < FileSize; ++i)
		{
			if (FileContent[i] == '\n')
			{
				++StringListSize;
			}
		}
	}
	else //Ошибка открытия файла
	{
		strerror(errno);
	}
}
//-----------------------------------------------------------------------------
void CreateStringList()
{
	char **StringList = (char **)malloc(sizeof(char *) * StringListSize);
	if (StringList)
	{
		size_t PosStart = 0, CurrentRow = 0;
		for (size_t j = 0; j < FileSize; ++j)
		{
			if (FileContent[j] == '\n')
			{
				size_t StringSize = j - PosStart;
				char *String = (char *)malloc(sizeof(char) * StringSize);
				if (String)
				{
					memset(String, 0, sizeof(char) * StringSize);
					strncpy(String, FileContent + PosStart, StringSize);
					String[StringSize] = '\0';
					StringList[CurrentRow] = String;
					PosStart = j + 1;
					++CurrentRow;
				}
			}
		}
	}
}
//-----------------------------------------------------------------------------
char** SearchFiles(const char *DirPath)
{
	
}
//-----------------------------------------------------------------------------
