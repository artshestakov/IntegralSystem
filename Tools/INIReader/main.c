#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
//-----------------------------------------------------------------------------
void ShowUsing(); //Показать инструкцию по использованию
//-----------------------------------------------------------------------------
int main(int argc, char **argv)
{
	bool Result = argc == 4 ? true : false;
	if (Result) //Параметры указаны верно
	{
		const char *SectionName = argv[1]; //Имя секции
		const char *ParameterName = argv[2]; //Имя параметра
		const char *FilePath = argv[3]; //Путь к конфигурационному файлу

		FILE *File = fopen(FilePath, "r");
		Result = File ? true : false;
		if (Result) //Файл открыт
		{
			char Line[1024];
			while (fgets(&Line, 1024, File) != NULL) //Обход каждой строки файла
			{
				size_t LineSize = strlen(Line); //Размер текущей строки
				if (Line[LineSize - 1] == '\n') //
				{

				}
			}
			fclose(File);
		}
		else //При открытии файла произошла ошибка
		{
			printf("Error open file %s: %s\n", FilePath, strerror(errno));
		}
	}
	else //Параметры указаны неверно
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
