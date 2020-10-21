#include "ISDictionaryWord.h"
#include "ISConstants.h"
#include "ISQuery.h"
//-----------------------------------------------------------------------------
static QString QS_WORD = PREPARE_QUERY("SELECT dicw_word "
									   "FROM _dictionaryword "
									   "WHERE NOT dicw_isdeleted "
									   "AND dicw_creationuser = currentuserid()");
//-----------------------------------------------------------------------------
static QString QI_WORD = PREPARE_QUERY("INSERT INTO _dictionaryword(dicw_word) "
									   "VALUES(:Word)");
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
	bool Result = ReadFromLocal();
	if (Result)
	{
		Result = ReadFromDB();
	}
	return Result;
}
//-----------------------------------------------------------------------------
bool ISDictionaryWord::Search(const char *String)
{
	//Обходим весь массив
	for (size_t i = 0; i < ArraySize; ++i)
	{
		if (strcmp(String, Array[i]) == 0) //Если нашли слово в массиве - выходим с положительным результатом
		{
			return true;
		}
	}

	//Обходим пользовательский массив
	for (const QString &Word : ArrayUser)
	{
		if (Word == QString::fromLocal8Bit(String)) //Если нашли слово в пользовательском массиве - выходим с положительным результатом
		{
			return true;
		}
	}

	//Слово не найдено ни в одном из массивов - выходим с отрицательным результатом
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
bool ISDictionaryWord::ReadFromLocal()
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
bool ISDictionaryWord::ReadFromDB()
{
	ISQuery qSelect(QS_WORD);
	bool Result = qSelect.Execute();
	if (Result)
	{
		while (qSelect.Next())
		{
			ArrayUser.emplace_back(qSelect.ReadColumn("dicw_word").toString());
		}
	}
	else
	{
		ErrorString = qSelect.GetErrorString();
	}
	return Result;
}
//-----------------------------------------------------------------------------
