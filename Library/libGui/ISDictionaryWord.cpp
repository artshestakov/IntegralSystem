#include "ISDictionaryWord.h"
#include "ISConstants.h"
//-----------------------------------------------------------------------------
#define WORD_SIZE 40
//-----------------------------------------------------------------------------
ISDictionaryWord::ISDictionaryWord()
	: ErrorString(NO_ERROR_STRING),
	Array(NULL),
	ArraySize(0)
{

}
//-----------------------------------------------------------------------------
ISDictionaryWord::~ISDictionaryWord()
{
	for (size_t i = 0; i < ArraySize; ++i)
	{
		free(Array[i]);
	}
	free(Array);
}
//-----------------------------------------------------------------------------
ISDictionaryWord& ISDictionaryWord::Instance()
{
	static ISDictionaryWord DictionaryWord;
	return DictionaryWord;
}
//-----------------------------------------------------------------------------
bool ISDictionaryWord::Initialize()
{
	FILE *File = fopen("C:\\Github\\IntegralSystem\\Resources\\Dictionary\\Russian.txt", "r");
	if (!File)
	{
		ErrorString = strerror(errno);
		return false;
	}
	
	//Количество слов, текущий символ, прочитано, размер текущей строки
	size_t Strings = 0, Char = 0;

	//Считаем количество слов
	while (!feof(File))
	{
		Char = fgetc(File);
		if (Char == '\n')
		{
			++Strings;
		}
	}

	//Смещаем каретку в начало файла для его чтения
	rewind(File);

	//Выделяем память под массив
	Array = (char **)malloc(sizeof(char *) * Strings);
	if (Array)
	{
		memset(Array, 0, Strings);
	}
	else //Ошибка выделения памяти
	{
		ErrorString = "Error malloc";
		return false;
	}
	
	//Временная переменная куда будет читать очередное слово
	char *String = (char *)malloc(WORD_SIZE + 1);
	if (String)
	{
		memset(String, 0, WORD_SIZE + 1);
	}
	else //Ошибка выделения памяти
	{
		ErrorString = "Error malloc";
		return false;
	}

	//Читаем файл построчно
	while (fgets(String, WORD_SIZE + 1, File))
	{
		size_t Size = strlen(String); //Размер текущего слова
		String[Size - 1] = '\0'; //Удаляем перенос строки
		Array[ArraySize] = (char *)malloc(Size); //Выделяем память в элементе массива
		if (Array[ArraySize]) //Память выделенна успешно - копируем текущее слово в массив и увеличиваем счётчик
		{
			strcpy(Array[ArraySize], String);
			++ArraySize;
		}
		else //Ошибка выделения памяти
		{
			ErrorString = "Error malloc";
			return false;
		}
	}

	//Освобождаем временную переменную и закрываем файл
	free(String);
	fclose(File);
	return true;
}
//-----------------------------------------------------------------------------
bool ISDictionaryWord::Search(const char *String)
{
	for (size_t i = 0; i < ArraySize; ++i) //Обходим весь массив
	{
		if (strcmp(String, Array[i]) == 0) //Если нашли слово в массиве - выходим с положительным результатом
		{
			return true;
		}
	}

	//Слово в массиве не найдено - выходим с отрицательным результатом
	return false;
}
//-----------------------------------------------------------------------------
