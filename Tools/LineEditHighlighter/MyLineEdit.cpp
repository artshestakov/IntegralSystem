#include "MyLineEdit.h"
#include <QtCore/QCoreApplication>
#include <QtCore/QTime>
#include <QtCore/QDebug>
#include <string.h>
//-----------------------------------------------------------------------------
const char SYMBOL_PUNCTS[] = " !\"#$%&'()*+,-./:;<=>?@[\\]^_`{|}~–0123456789";
const size_t SYMBOL_PUNCTS_SIZE = strlen(SYMBOL_PUNCTS);
const char SYMBOL_DIGITS[] = "0123456789";
const size_t SYMBOL_DIGITS_SIZE = strlen(SYMBOL_DIGITS);
//-----------------------------------------------------------------------------
const ISIntArrayItem INT_ARRAY[] = 
{
	{ -64, -32 }, //А
	{ -63, -31 }, //Б
	{ -62, -30 }, //В
	{ -61, -29 }, //Г
	{ -60, -28 }, //Д
	{ -59, -27 }, //Е
	{ -88, -72 }, //Ё
	{ -58, -26 }, //Ж
	{ -57, -25 }, //З
	{ -56, -24 }, //И
	{ -55, -23 }, //Й
	{ -54, -22 }, //К
	{ -53, -21 }, //Л
	{ -52, -20 }, //М
	{ -51, -19 }, //Н
	{ -50, -18 }, //О
	{ -49, -17 }, //П
	{ -48, -16 }, //Р
	{ -47, -15 }, //С
	{ -46, -14 }, //Т
	{ -45, -13 }, //У
	{ -44, -12 }, //Ф
	{ -43, -11 }, //Х
	{ -42, -10 }, //Ц
	{ -41, -9 }, //Ч
	{ -40, -8 }, //Ш
	{ -39, -7 }, //Щ
	{ -38, -6 }, //Ъ
	{ -37, -5 }, //Ы
	{ -36, -4 }, //Ь
	{ -35, -3 }, //Э
	{ -34, -2 }, //Ю
	{ -33, -1 }, //Я

};
const size_t INT_ARRAY_SIZE = 33;
//-----------------------------------------------------------------------------
static int comparisonFunc(const void *c1, const void *c2)
{
	return strcmp(*(const char **)c1, *(const char **)c2);
}
//-----------------------------------------------------------------------------
static void SetTextFormat(QLineEdit* LineEdit, const QList<QTextLayout::FormatRange>& Formats)
{
	QList<QInputMethodEvent::Attribute> Attributes;
	for (const QTextLayout::FormatRange &Format : Formats)
	{
		Attributes.append(QInputMethodEvent::Attribute(QInputMethodEvent::TextFormat, Format.start - LineEdit->cursorPosition(), Format.length, Format.format));
	}
	QInputMethodEvent InputMethodEvent(QString(), Attributes);
	QCoreApplication::sendEvent(LineEdit, &InputMethodEvent);
}
//-----------------------------------------------------------------------------
static void ClearTextFormat(QLineEdit* lineEdit)
{
	SetTextFormat(lineEdit, QList<QTextLayout::FormatRange>());
}
//-----------------------------------------------------------------------------
MyLineEdit::MyLineEdit(QWidget *parent)
	: QLineEdit(parent),
	StringListSize(0)
{
	setFixedHeight(24);
	connect(this, &MyLineEdit::textChanged, this, &MyLineEdit::TextChanged);

	FILE *File = fopen("G:\\Github\\IntegralSystem\\Resources\\Dictionary\\russian.txt", "r");
	if (File)
	{
		fseek(File, 0, SEEK_END);
		long FileSize = ftell(File);
		rewind(File);

		char *Buffer = (char *)malloc(FileSize + 1);
		fread(Buffer, sizeof(char), FileSize, File);
		fclose(File);

		for (size_t i = 0; i < FileSize; ++i)
		{
			if (Buffer[i] == '\n')
			{
				++StringListSize;
			}
		}
		StringList = (char **)malloc(sizeof(char *) * StringListSize);

		size_t LastPos = 0, Index = 0;
		for (size_t i = 0; i < FileSize; ++i)
		{
			if (Buffer[i] == '\n')
			{
				size_t StringSize = i - LastPos;
				char *String = (char *)malloc(StringSize + 1);
				memset(String, 0, StringSize);
				strncpy(String, Buffer + LastPos, i - LastPos);
				String[i - LastPos] = '\0';
				for (size_t j = 0, c = strlen(String); j < c; ++j)
				{
					if (String[j] == '\n')
					{
						printf("%s", String);
					}
				}
				LastPos = i + 1;
				StringList[Index] = String;
				++Index;
			}
		}
	}
	setText("hel out");
}
//-----------------------------------------------------------------------------
MyLineEdit::~MyLineEdit()
{

}
//-----------------------------------------------------------------------------
void MyLineEdit::TextChanged(const QString &Text)
{
	if (Text.isEmpty()) //Если текста нет - удаляем предыдущие форматы и выходим из функции
	{
		ClearTextFormat(this);
		return;
	}
	
	//Выделяем память и копируем значение в память
	char *String = (char *)malloc(Text.size() + 1);
	strcpy(String, Text.toLocal8Bit().constData());
	size_t StringSize = strlen(String);

	//Объявляем массив точек и его размер
	ISPoint **Points = NULL;
	size_t PointSize = 0;

	if (CountPunct(String, StringSize) > 0) //Если в строке есть знаки препинания - обрабатываем основным алгоритмом
	{
		size_t CurrentWordSize = 0;
		for (size_t i = 0; i < StringSize; ++i) //Обходим обрабатываемую строку
		{
			bool IsPunctCurrentChar = IsPunct(String[i]);
			if (!IsPunctCurrentChar) //Если текущий символ не знак пунктуации - инкрементируем размер слова
			{
				++CurrentWordSize;
				if (i != StringSize - 1) //Если текущая итерация не последняя - продолжаем поиск букв
				{
					continue;
				}
			}

			if (!CurrentWordSize && IsPunctCurrentChar) //Если размер слова равен нулю и текущий символ знак пунтуации - переходим к следующей итерации
			{
				continue;
			}

			if (IsPunct(String[i - 1])) //Если предыдущий символ является знаком пунктуации
			{
				Points = ReallocPoints(Points, PointSize);
				Points[PointSize - 1] = CreatePoint(i, CurrentWordSize);
			}
			else //Предыдущий символ не явлеяется знаком пунктуации
			{
				Points = ReallocPoints(Points, PointSize);
				size_t StartPos = i - CurrentWordSize;
				if (StartPos)
				{
					++StartPos;
				}
				Points[PointSize - 1] = CreatePoint(StartPos, CurrentWordSize);
				CurrentWordSize = 0;
			}
		}
	}
	else //В строке нет знаков препинания - считаем что слово одно
	{
		Points = ReallocPoints(Points, PointSize);
		Points[PointSize - 1] = CreatePoint(0, StringSize);
	}

	QList<QTextLayout::FormatRange> Formats;
	for (size_t i = 0; i < PointSize; ++i) //Обходим все точки и проверяем орфографию
	{
		ISPoint *Point = Points[i];
		char *Word = (char *)malloc(Point->Size + 1);
		if (Point->Size == 1) //Если слово состоит из одной буквы
		{
			Word[0] = String[Point->Pos];
		}
		else //Слово состоит больше чем из одной буквы
		{
			strncpy(Word, String + Point->Pos + 1, Point->Size);
		}
		Word[Point->Size] = '\0';
		ToLowerString(Word, Point->Size); //Приведение к нижнему регистру
		if (!IsDigit(Word, Point->Size)) //Если слово действительно является словом
		{
			if (!ExistDictionary(Word)) //Если такое слово в словаре отсутствует - включаем подветку
			{
				Formats.append(CreateFormatRange(Point->Pos, Point->Size));
			}
		}
		free(Word);
	}
	SetTextFormat(this, Formats); //Установка формата

	//Обсвобождаем память
	free(String);
	for (size_t i = 0; i < PointSize; ++i)
	{
		free(Points[i]);
	}
	free(Points);
}
//-----------------------------------------------------------------------------
bool MyLineEdit::ExistDictionary(const char *String)
{
	bool Result = false;
	for (size_t i = 0; i < StringListSize; ++i) //Обходим весь словарь
	{
		Result = strcmp(String, StringList[i]) == 0;
		if (Result) //Если нашли такое слово - выходим
		{
			break;
		}
	}
	return Result;
}
//-----------------------------------------------------------------------------
size_t MyLineEdit::CountPunct(const char *String, size_t Size)
{
	size_t Count = 0;
	for (size_t i = 0; i < Size; ++i) //Обходим строку
	{
		if (IsPunct(String[i])) //Попался знак препинания - инкрементируем счётчик
		{
			++Count;
		}
	}
	return Count;
}
//-----------------------------------------------------------------------------
ISPoint** MyLineEdit::ReallocPoints(ISPoint **Points, size_t &PointSize)
{
	return Points ? (ISPoint **)realloc(Points, sizeof(struct ISPoint *) * ++PointSize) : (ISPoint **)malloc(sizeof(struct ISPoint *) * ++PointSize);
}
//-----------------------------------------------------------------------------
ISPoint* MyLineEdit::CreatePoint(size_t Start, size_t End)
{
	ISPoint *Point = (ISPoint *)malloc(sizeof(struct ISPoint));
	Point->Pos = Start;
	Point->Size = End;
	return Point;
}
//-----------------------------------------------------------------------------
QTextLayout::FormatRange MyLineEdit::CreateFormatRange(int Start, int Lenght)
{
	QTextCharFormat TextCharFormat;
	TextCharFormat.setUnderlineColor(Qt::red);
	TextCharFormat.setFontUnderline(true);
	return QTextLayout::FormatRange{ Start, Lenght, TextCharFormat };
}
//-----------------------------------------------------------------------------
void MyLineEdit::ToLowerString(char *String, size_t Size)
{
	for (size_t i = 0; i < Size; ++i)
	{
		char Char = String[i];
		if (Char >= -1 && Char <= 255) //Если текущий символ не принадлежит к русскому алфавиту
		{
			String[i] = tolower(String[i]);
		}
		else //Текущий символ принадлежит к русскому алфавиту
		{
			for (size_t j = 0; j < INT_ARRAY_SIZE; ++j)
			{
				if (INT_ARRAY[j].Key == Char)
				{
					String[i] = INT_ARRAY[j].Value;
					break;
				}
			}
		}
	}
}
//-----------------------------------------------------------------------------
bool MyLineEdit::IsDigit(const char *Word, size_t Size)
{
	bool Result = true;
	for (size_t i = 0; i < Size; ++i)
	{
		Result = IsDigit(Word[i]);
		if (!Result)
		{
			break;
		}
	}
	return Result;
}
//-----------------------------------------------------------------------------
bool MyLineEdit::IsDigit(int Char)
{
	bool Result = Char >= -1 && Char <= 255;
	if (Result) //Если символ не принадлежит к русскому алфавиту
	{
		Result = isdigit(Char);
	}
	else //Символ принадлежит к русскому алфавиту
	{
		for (size_t i = 0; i < SYMBOL_DIGITS_SIZE; ++i) //Обходим массив чисел
		{
			Result = SYMBOL_DIGITS[i] == Char;
			if (Result)
			{
				break;
			}
		}
	}
	return Result;
}
//-----------------------------------------------------------------------------
bool MyLineEdit::IsPunct(int Char)
{
	bool Result = true;
	for (size_t i = 0; i < SYMBOL_PUNCTS_SIZE; ++i) //Обходим массив знаков препинания
	{
		Result = SYMBOL_PUNCTS[i] == Char;
		if (Result)
		{
			break;
		}
	}
	if (!Result)
	{
		if (Char >= -1 && Char <= 255) //Если символ не принадлежит к русскому алфавиту - проверяем системной функцией
		{
			Result = ispunct(Char);
		}
	}
	return Result;
}
//-----------------------------------------------------------------------------
