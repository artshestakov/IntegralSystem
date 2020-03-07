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
	
	//���������� ����, ������� ������, ���������, ������ ������� ������
	size_t Strings = 0, Char = 0;

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

	//�������� ������ ��� ������
	Array = (char **)malloc(sizeof(char *) * Strings);
	if (Array)
	{
		memset(Array, 0, Strings);
	}
	else //������ ��������� ������
	{
		ErrorString = "Error malloc";
		return false;
	}
	
	//��������� ���������� ���� ����� ������ ��������� �����
	char *String = (char *)malloc(WORD_SIZE + 1);
	if (String)
	{
		memset(String, 0, WORD_SIZE + 1);
	}
	else //������ ��������� ������
	{
		ErrorString = "Error malloc";
		return false;
	}

	//������ ���� ���������
	while (fgets(String, WORD_SIZE + 1, File))
	{
		size_t Size = strlen(String); //������ �������� �����
		String[Size - 1] = '\0'; //������� ������� ������
		Array[ArraySize] = (char *)malloc(Size); //�������� ������ � �������� �������
		if (Array[ArraySize]) //������ ��������� ������� - �������� ������� ����� � ������ � ����������� �������
		{
			strcpy(Array[ArraySize], String);
			++ArraySize;
		}
		else //������ ��������� ������
		{
			ErrorString = "Error malloc";
			return false;
		}
	}

	//����������� ��������� ���������� � ��������� ����
	free(String);
	fclose(File);
	return true;
}
//-----------------------------------------------------------------------------
bool ISDictionaryWord::Search(const char *String)
{
	for (size_t i = 0; i < ArraySize; ++i) //������� ���� ������
	{
		if (strcmp(String, Array[i]) == 0) //���� ����� ����� � ������� - ������� � ������������� �����������
		{
			return true;
		}
	}

	//����� � ������� �� ������� - ������� � ������������� �����������
	return false;
}
//-----------------------------------------------------------------------------
