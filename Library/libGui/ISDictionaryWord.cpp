#include "ISDictionaryWord.h"
#include "ISConstants.h"
//-----------------------------------------------------------------------------
ISDictionaryWord::ISDictionaryWord()
	: ErrorString(NO_ERROR_STRING)
{

}
//-----------------------------------------------------------------------------
ISDictionaryWord::~ISDictionaryWord()
{

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
	FILE *File = fopen("", "r");
	bool Result = File ? true : false;
	if (Result)
	{
		size_t Strings = 0; //Количество слов
		int Char = 0; //Текущий символ

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

		fclose(File);
	}
	else
	{
		ErrorString = strerror(errno);
	}
	return Result;
}
//-----------------------------------------------------------------------------
