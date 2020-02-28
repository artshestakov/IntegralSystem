#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
//-----------------------------------------------------------------------------
char *FilePath = NULL; //Путь к файлу
char *StringTemplate = NULL; //Поисковое значение (шаблон)
char *StringReplace = NULL; //Заменяемое значение
char *StringContent = NULL; //Содержимое файла
char *StringResult = NULL; //Итоговое содержимое файла
//-----------------------------------------------------------------------------
void Using(); //Вывод инструкции в консоль
char *Replace(const char *String, const char *OldString, const char *NewString); //Замена строки
//-----------------------------------------------------------------------------
int main(int argc, char **argv)
{
	bool Result = argc == 4;
	if (Result) //Количество аргументов правильное
	{
		FilePath = argv[1];
		StringTemplate = argv[2];
		StringReplace = argv[3];

		FILE *File = fopen(FilePath, "rt"); //Открываем файл для чтения
		Result = File ? true : false;
		if (Result) //Файл открылся успешно
		{
			fseek(File, 0, SEEK_END);
			long FileSize = ftell(File);
			rewind(File);

			StringContent = (char *)malloc(FileSize + 1);
			Result = StringContent ? true : false;
			if (Result) //Память выделена успешно
			{
				memset(StringContent, 0, FileSize);
				fread(StringContent, sizeof(char), FileSize, File);
				StringContent[FileSize] = '\0';
			}
			else //Ошибка выделения памяти
			{
				printf("Error malloc\n");
			}
			fclose(File);
		}
		else //Ошибка открытия файла
		{
			printf("Error open file \"%s\": %s\n", FilePath, strerror(errno));
		}
	}
	else //Количество аргументов не правильное
	{
		printf("Invalid arguments\n");
		Using();
	}

	//Если все предыдущие операции были выполнены успешно - производим замену по шаблону и перезаписываем файл
	if (Result)
	{
		StringResult = Replace(StringContent, StringTemplate, StringReplace);
		if (strcmp(StringResult, StringContent) == 0) //Если строки равны - искомое значение не найдено
		{
			printf("Not found string template \"%s\"\n", StringTemplate);
		}
		else
		{
			FILE *File = fopen(FilePath, "w+b");
			Result = File ? true : false;
			if (Result) //Файл успешно открылся
			{
				fwrite(StringResult, sizeof(char), strlen(StringResult), File);
				fclose(File);
				printf("Replacement was successful: %s -> %s\n", StringTemplate, StringReplace);
			}
			else
			{
				printf("Error write to file \"%s\": %s\n", FilePath, strerror(errno));
			}
		}
	}

	return Result ? EXIT_SUCCESS : EXIT_FAILURE;
}
//-----------------------------------------------------------------------------
void Using()
{
	printf("Utility for noting text in a file by template.\n");
	printf("Usage: Replacer");
#ifdef WIN32
	printf(".exe");
#endif
	printf(" [FILE_PATH] [STRING_TEMPLATE] [STRING_REPLACE]\n");
}
//-----------------------------------------------------------------------------
char* Replace(const char *String, const char *OldString, const char *NewString)
{
	char *Result;
	size_t Index, Count = 0;
	size_t NewSize = strlen(NewString);
	size_t OldSize = strlen(OldString);

	for (Index = 0; String[Index] != '\0'; Index++)
	{
		if (strstr(&String[Index], OldString) == &String[Index])
		{
			Count++;
			Index += OldSize - 1;
		}
	}

	Result = (char *)malloc(Index + Count * (NewSize - OldSize) + 1);

	Index = 0;
	while (*String)
	{
		if (strstr(String, OldString) == String)
		{
			strcpy(&Result[Index], NewString);
			Index += NewSize;
			String += OldSize;
		}
		else
		{
			Result[Index++] = *String++;
		}
	}

	Result[Index] = '\0';
	return Result;
}
//-----------------------------------------------------------------------------
