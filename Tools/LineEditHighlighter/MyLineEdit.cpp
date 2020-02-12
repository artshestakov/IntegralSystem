#include "MyLineEdit.h"
#include <QtCore/QCoreApplication>
#include <QtCore/QTime>
#include <QtCore/QDebug>
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
		QInputMethodEvent::AttributeType type = QInputMethodEvent::TextFormat;
		int start = Format.start - LineEdit->cursorPosition();
		int length = Format.length;
		QVariant value = Format.format;
		Attributes.append(QInputMethodEvent::Attribute(type, start, length, value));
	}
	QInputMethodEvent event(QString(), Attributes);
	QCoreApplication::sendEvent(LineEdit, &event);
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

	//Объявляем массив точек и его размер
	ISPoint **Points = NULL;
	size_t PointSize = 0;

	if (CountSpace(String, strlen(String)) > 0) //Если в строке есть пробелы - обрабатываем основным алгоритмом
	{
		size_t CurrentWordSize = 0;
		for (size_t i = 0, StringSize = strlen(String); i < StringSize; ++i) //Обходим обрабатываемую строку
		{
			if (String[i] != ' ')
			{
				++CurrentWordSize;
				continue;
			}

			if (!CurrentWordSize && String[i] == ' ')
			{
				continue;
			}

			if (String[i - 1] != ' ')
			{
				Points = Points ? (ISPoint **)realloc(Points, sizeof(struct ISPoint *) * ++PointSize) : (ISPoint **)malloc(sizeof(struct ISPoint *) * ++PointSize);
				Points[PointSize - 1] = CreatePoint(i - CurrentWordSize, i - 1);
				CurrentWordSize = 0;
			}
		}
	}
	else //В строке нет пробелов - считаем что слово одно
	{
		Points = (ISPoint **)malloc(sizeof(struct ISPoint *) * ++PointSize);
		Points[PointSize - 1] = CreatePoint(0, strlen(String));
	}

	QList<QTextLayout::FormatRange> Formats;
	for (size_t i = 0; i < PointSize; ++i) //Обходим все точки и проверяем орфографию
	{
		ISPoint *Point = Points[i];
		size_t WordSize = Point->End - Point->Start + 1;
		char *Word = (char *)malloc(WordSize + 1);
		strncpy(Word, String + Point->Start, Point->End - Point->Start + 1);
		WordSize = strlen(Word); //Обязательно
		Word[WordSize] = '\0';
		ToLowerString(Word, WordSize); //Приведение к нижнему регистру
		if (!IsDigit(Word, WordSize)) //Если слово действительно является словом
		{
			if (!ExistDictionary(Word)) //Если такое слово в словаре отсутствует - включаем подветку
			{
				Formats.append(CreateFormatRange(Point->Start, WordSize));
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
size_t MyLineEdit::CountSpace(const char *String, size_t Size)
{
	size_t Count = 0;
	for (size_t i = 0; i < Size; ++i) //Обходим строку
	{
		if (String[i] == ' ') //Попался пробел - инкрементируем счётчик
		{
			++Count;
		}
	}
	return Count;
}
//-----------------------------------------------------------------------------
ISPoint* MyLineEdit::CreatePoint(size_t Start, size_t End)
{
	ISPoint *Point = (ISPoint *)malloc(sizeof(struct ISPoint));
	Point->Start = Start;
	Point->End = End;
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
		String[i] = tolower(String[i]);
	}
}
//-----------------------------------------------------------------------------
bool MyLineEdit::IsDigit(const char *Word, size_t Size)
{
	bool Result = true;
	for (size_t i = 0; i < Size; ++i)
	{
		Result = isdigit(Word[i]) != 0;
		if (!Result)
		{
			break;
		}
	}
	return Result;
}
//-----------------------------------------------------------------------------
