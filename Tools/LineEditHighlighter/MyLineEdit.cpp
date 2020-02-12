#include "MyLineEdit.h"
#include <QtGui/QTextLayout>
#include <QtCore/QCoreApplication>
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
	//setText("Task Tracker - Entry");
	connect(this, &MyLineEdit::textChanged, this, &MyLineEdit::TextChanged);

	/*QList<QTextLayout::FormatRange> formats;

	QTextCharFormat TextCharFormat;
	TextCharFormat.setUnderlineColor(Qt::red);
	TextCharFormat.setFontUnderline(true);

	QTextLayout::FormatRange TextLayout;
	TextLayout.start = 0;
	TextLayout.length = 4;
	TextLayout.format = TextCharFormat;

	formats.append(TextLayout);

	SetTextFormat(this, formats);*/

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
	if (Text.isEmpty())
	{
		ClearTextFormat(this);
		return;
	}

	char *String = (char *)malloc(Text.size() + 1); //Выделяем память под обрабатываемую строку
	if (!String)
	{
		return;
	}

	strcpy(String, Text.toLocal8Bit().constData()); //Копируем текущую строку в выделенную память

	ISPoint **Points = NULL;
	size_t PointSize = 0;

	if (CountSpace(String, strlen(String)) > 0)
	{
		size_t PosStart = 0, CurrentWordSize = 0;
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

			PosStart = i - CurrentWordSize;
			if (String[i - 1] != ' ')
			{
				Points = Points ? (ISPoint **)realloc(Points, sizeof(struct ISPoint *) * ++PointSize) : (ISPoint **)malloc(sizeof(struct ISPoint *) * ++PointSize);
				Points[PointSize - 1] = (ISPoint *)malloc(sizeof(struct ISPoint));
				Points[PointSize - 1]->Start = PosStart;
				Points[PointSize - 1]->End = i - 1;
				CurrentWordSize = 0;
			}
		}
	}
	else
	{
		Points = (ISPoint **)malloc(sizeof(struct ISPoint *) * ++PointSize);
		Points[PointSize - 1] = (ISPoint *)malloc(sizeof(struct ISPoint));
		Points[PointSize - 1]->Start = 0;
		Points[PointSize - 1]->End = strlen(String);
	}

	QList<QTextLayout::FormatRange> Formats;
	
	QTextCharFormat TextCharFormat;
	TextCharFormat.setUnderlineColor(Qt::red);
	TextCharFormat.setFontUnderline(true);

	for (size_t i = 0; i < PointSize; ++i)
	{
		ISPoint *Point = Points[i];
		size_t WordSize = Point->End - Point->Start + 1;
		char *Word = (char *)malloc(WordSize + 1);
		strncpy(Word, String + Point->Start, Point->End - Point->Start + 1);
		Word[WordSize] = '\0';

		if (!ExistDictionary(Word))
		{
			QTextLayout::FormatRange TextLayout;
			TextLayout.start = Point->Start;
			TextLayout.length = WordSize;
			TextLayout.format = TextCharFormat;
			Formats.append(TextLayout);
		}
		free(Word);
	}

	if (Formats.isEmpty())
	{
		ClearTextFormat(this);
	}
	else
	{
		SetTextFormat(this, Formats);
	}

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
	for (size_t i = 0; i < StringListSize; ++i)
	{
		Result = strcmp(String, StringList[i]) == 0;
		if (Result)
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
	for (size_t i = 0; i < Size; ++i)
	{
		if (String[i] == ' ')
		{
			++Count;
		}
	}
	return Count;
}
//-----------------------------------------------------------------------------
