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
		size_t Strings = 0; //���������� ����
		int Char = 0; //������� ������

		//������� ���������� ����
		while (!feof(File))
		{
			Char = fgetc(File);
			if (Char == '\n')
			{
				++Strings;
			}
		}

		//������� ������� � ������ ����� ��� ��� ������
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
