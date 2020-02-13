#include "MyLineEdit.h"
#include <QtCore/QCoreApplication>
#include <QtCore/QTime>
#include <QtCore/QDebug>
#include <string.h>
//-----------------------------------------------------------------------------
const char SYMBOL_PUNCTS[] = " !\"#$%&'()*+,-./:;<=>?@[\\]^_`{|}~�0123456789";
const size_t SYMBOL_PUNCTS_SIZE = strlen(SYMBOL_PUNCTS);
const char SYMBOL_DIGITS[] = "0123456789";
const size_t SYMBOL_DIGITS_SIZE = strlen(SYMBOL_DIGITS);
//-----------------------------------------------------------------------------
const ISIntArrayItem INT_ARRAY[] = 
{
	{ -64, -32 }, //�
	{ -63, -31 }, //�
	{ -62, -30 }, //�
	{ -61, -29 }, //�
	{ -60, -28 }, //�
	{ -59, -27 }, //�
	{ -88, -72 }, //�
	{ -58, -26 }, //�
	{ -57, -25 }, //�
	{ -56, -24 }, //�
	{ -55, -23 }, //�
	{ -54, -22 }, //�
	{ -53, -21 }, //�
	{ -52, -20 }, //�
	{ -51, -19 }, //�
	{ -50, -18 }, //�
	{ -49, -17 }, //�
	{ -48, -16 }, //�
	{ -47, -15 }, //�
	{ -46, -14 }, //�
	{ -45, -13 }, //�
	{ -44, -12 }, //�
	{ -43, -11 }, //�
	{ -42, -10 }, //�
	{ -41, -9 }, //�
	{ -40, -8 }, //�
	{ -39, -7 }, //�
	{ -38, -6 }, //�
	{ -37, -5 }, //�
	{ -36, -4 }, //�
	{ -35, -3 }, //�
	{ -34, -2 }, //�
	{ -33, -1 }, //�

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
	if (Text.isEmpty()) //���� ������ ��� - ������� ���������� ������� � ������� �� �������
	{
		ClearTextFormat(this);
		return;
	}
	
	//�������� ������ � �������� �������� � ������
	char *String = (char *)malloc(Text.size() + 1);
	strcpy(String, Text.toLocal8Bit().constData());
	size_t StringSize = strlen(String);

	//��������� ������ ����� � ��� ������
	ISPoint **Points = NULL;
	size_t PointSize = 0;

	if (CountPunct(String, StringSize) > 0) //���� � ������ ���� ����� ���������� - ������������ �������� ����������
	{
		size_t CurrentWordSize = 0;
		for (size_t i = 0; i < StringSize; ++i) //������� �������������� ������
		{
			bool IsPunctCurrentChar = IsPunct(String[i]);
			if (!IsPunctCurrentChar) //���� ������� ������ �� ���� ���������� - �������������� ������ �����
			{
				++CurrentWordSize;
				if (i != StringSize - 1) //���� ������� �������� �� ��������� - ���������� ����� ����
				{
					continue;
				}
			}

			if (!CurrentWordSize && IsPunctCurrentChar) //���� ������ ����� ����� ���� � ������� ������ ���� ��������� - ��������� � ��������� ��������
			{
				continue;
			}

			if (IsPunct(String[i - 1])) //���� ���������� ������ �������� ������ ����������
			{
				Points = ReallocPoints(Points, PointSize);
				Points[PointSize - 1] = CreatePoint(i, CurrentWordSize);
			}
			else //���������� ������ �� ��������� ������ ����������
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
	else //� ������ ��� ������ ���������� - ������� ��� ����� ����
	{
		Points = ReallocPoints(Points, PointSize);
		Points[PointSize - 1] = CreatePoint(0, StringSize);
	}

	QList<QTextLayout::FormatRange> Formats;
	for (size_t i = 0; i < PointSize; ++i) //������� ��� ����� � ��������� ����������
	{
		ISPoint *Point = Points[i];
		char *Word = (char *)malloc(Point->Size + 1);
		if (Point->Size == 1) //���� ����� ������� �� ����� �����
		{
			Word[0] = String[Point->Pos];
		}
		else //����� ������� ������ ��� �� ����� �����
		{
			strncpy(Word, String + Point->Pos + 1, Point->Size);
		}
		Word[Point->Size] = '\0';
		ToLowerString(Word, Point->Size); //���������� � ������� ��������
		if (!IsDigit(Word, Point->Size)) //���� ����� ������������� �������� ������
		{
			if (!ExistDictionary(Word)) //���� ����� ����� � ������� ����������� - �������� ��������
			{
				Formats.append(CreateFormatRange(Point->Pos, Point->Size));
			}
		}
		free(Word);
	}
	SetTextFormat(this, Formats); //��������� �������

	//������������ ������
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
	for (size_t i = 0; i < StringListSize; ++i) //������� ���� �������
	{
		Result = strcmp(String, StringList[i]) == 0;
		if (Result) //���� ����� ����� ����� - �������
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
	for (size_t i = 0; i < Size; ++i) //������� ������
	{
		if (IsPunct(String[i])) //������� ���� ���������� - �������������� �������
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
		if (Char >= -1 && Char <= 255) //���� ������� ������ �� ����������� � �������� ��������
		{
			String[i] = tolower(String[i]);
		}
		else //������� ������ ����������� � �������� ��������
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
	if (Result) //���� ������ �� ����������� � �������� ��������
	{
		Result = isdigit(Char);
	}
	else //������ ����������� � �������� ��������
	{
		for (size_t i = 0; i < SYMBOL_DIGITS_SIZE; ++i) //������� ������ �����
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
	for (size_t i = 0; i < SYMBOL_PUNCTS_SIZE; ++i) //������� ������ ������ ����������
	{
		Result = SYMBOL_PUNCTS[i] == Char;
		if (Result)
		{
			break;
		}
	}
	if (!Result)
	{
		if (Char >= -1 && Char <= 255) //���� ������ �� ����������� � �������� �������� - ��������� ��������� ��������
		{
			Result = ispunct(Char);
		}
	}
	return Result;
}
//-----------------------------------------------------------------------------
