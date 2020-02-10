#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
//-----------------------------------------------------------------------------
void ShowUsing(); //Показать инструкцию по использованию
bool IsLineSection(const char *String, size_t Size); //Проверка строки на секцию
bool IsLineParameter(const char *String, size_t Size); //Проверка строки на параметр
bool IsLineComment(const char *String); //Проверки строки на комментарий
char* ExtractSectionName(const char *String, size_t Size); //Вытащить название секции из строки
char* ExtractParameterName(const char *String, size_t Size); //Вытащить название параметра из строки
char* ExtractParameterValue(const char *String, size_t Size); //Вытащить значение параметра из строки
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
			char *CurrentSectionName = NULL;
			char *CurrentParameterName = NULL;
			while (fgets(Line, 1024, File) != NULL) //Обход каждой строки файла
			{
				if (!IsLineComment(Line))
				{
					size_t LineSize = strlen(Line);
					if (IsLineSection(Line, LineSize))
					{
						CurrentSectionName = ExtractSectionName(Line, LineSize);
					}
					else if (IsLineParameter(Line, LineSize))
					{
						CurrentParameterName = ExtractParameterName(Line, LineSize);
					}

					if (CurrentSectionName && CurrentParameterName)
					{
						if (strcmp(CurrentSectionName, SectionName) == 0 && strcmp(CurrentParameterName, ParameterName) == 0)
						{
							char *ParameterValue = ExtractParameterValue(Line, LineSize);
							printf("%s", ParameterValue);
							break;
						}
					}
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
bool IsLineSection(const char *String, size_t Size)
{
	bool BracketOpen = false, BracketClose = false;
	for (size_t i = 0; i < Size; ++i)
	{
		if (String[i] == '[')
		{
			BracketOpen = true;
			break;
		}
	}
	for (size_t i = 0; i < Size; ++i)
	{
		if (String[i] == ']')
		{
			BracketClose = true;
			break;
		}
	}
	return BracketOpen && BracketClose;
}
//-----------------------------------------------------------------------------
bool IsLineParameter(const char *String, size_t Size)
{
	bool Result = false;
	for (size_t i = 0; i < Size; ++i)
	{
		Result = String[i] == '=';
		if (Result)
		{
			break;
		}
	}
	return Result;
}
//-----------------------------------------------------------------------------
bool IsLineComment(const char *String)
{
	bool Result = true;
	if (String)
	{
		Result = String[0] == ';' || String[0] == '#';
	}
	return Result;
}
//-----------------------------------------------------------------------------
char* ExtractSectionName(const char *String, size_t Size)
{
	char *SectionName = NULL;
	size_t Pos = 0;
	for (size_t i = 0; i < Size; ++i) //Обход всей строки
	{
		if (String[i] == '[') //Нашли открывающуюся скобку
		{
			Pos = i + 1; //Запомнили начальную позицию имени
		}
		else if (String[i] == ']') //Когда нашли закрывающуюся скобку - считаем что название есть и можно его вытаскивать
		{
			SectionName = (char *)malloc(sizeof(char) * (i - Pos));
			strncpy(SectionName, String + Pos, i - Pos);
			SectionName[i - Pos] = '\0';
			break;
		}
	}
	return SectionName;
}
//-----------------------------------------------------------------------------
char* ExtractParameterName(const char *String, size_t Size)
{
	char *ParameterName = NULL;
	for (size_t i = 0; i < Size; ++i)
	{
		if (String[i] == '=')
		{
			ParameterName = (char *)malloc(sizeof(char) * i);
			strncpy(ParameterName, String, i);
			ParameterName[i] = '\0';
			break;
		}
	}
	return ParameterName;
}
//-----------------------------------------------------------------------------
char* ExtractParameterValue(const char *String, size_t Size)
{
	char *ParameterValue = NULL;
	for (size_t i = 0; i < Size; ++i)
	{
		if (String[i] == '=') //Нашли символ приравнивания значения
		{
			ParameterValue = (char *)malloc(sizeof(char) * (Size - i));
			strncpy(ParameterValue, String + i + 1, Size - i);
			ParameterValue[Size - i] = '\0';
			break;
		}
	}

	for (size_t i = 0, ParameterSize = strlen(ParameterValue); i < ParameterSize; ++i)
	{
		if (ParameterValue[i] == ';' || ParameterValue[i] == '#')
		{
			memset(ParameterValue + i, 0, ParameterSize - i);
			break;
		}
	}

	if (ParameterValue[0] == '\"') //Если тип парамера строка (возможно с символами пробела)
	{
		for (size_t i = 1, ParameterSize = strlen(ParameterValue); i < ParameterSize; ++i)
		{
			if (ParameterValue[i] == '\"') //Нашли закрывающую кавычку - отбрасываем все что после закрывающей кавычки
			{
				memset(ParameterValue + i + 1, 0, ParameterSize - i - 1);
				break;
			}
		}
	}
	else //Тип параметра не строка
	{
		while (true) //Ищем и удаляем пробелы в конце строки
		{
			char LastChar = ParameterValue[strlen(ParameterValue) - 1];
			if (LastChar == ' ' || LastChar == '\n')
			{
				memset(ParameterValue + strlen(ParameterValue) - 1, 0, 1);
			}
			else
			{
				break;
			}
		}
	}

	return ParameterValue;
}
//-----------------------------------------------------------------------------
