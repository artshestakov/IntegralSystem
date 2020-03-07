#include "ISDictionaryWord.h"
#include "ISConstants.h"
#include "ISQuery.h"
//-----------------------------------------------------------------------------
static QString QI_WORD = PREPARE_QUERY("INSERT INTO _dictionaryword (dicw_user, dicw_word) "
									   "VALUES (currentuserid(), :Word)");
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
QString ISDictionaryWord::GetErrorString() const
{
	return ErrorString;
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
	//������� ���� ������
	for (size_t i = 0; i < ArraySize; ++i)
	{
		if (strcmp(String, Array[i]) == 0) //���� ����� ����� � ������� - ������� � ������������� �����������
		{
			return true;
		}
	}

	//������� ���������������� ������
	for (size_t i = 0, c = ArrayUser.size(); i < c; ++i)
	{
		if (strcmp(String, ArrayUser[i]) == 0) //���� ����� ����� � ���������������� ������� - ������� � ������������� �����������
		{
			return true;
		}
	}

	//����� �� ������� �� � ����� �� �������� - ������� � ������������� �����������
	return false;
}
//-----------------------------------------------------------------------------
void ISDictionaryWord::AddWord(const QString &Word)
{
	ISQuery qInsert(QI_WORD);
	qInsert.BindValue(":Word", Word);
	if (qInsert.Execute())
	{
		ArrayUser.emplace_back(Word.toStdString().c_str());
	}
}
//-----------------------------------------------------------------------------
