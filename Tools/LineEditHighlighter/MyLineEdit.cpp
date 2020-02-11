#include "MyLineEdit.h"
#include <QtGui/QTextLayout>
#include <QtCore/QCoreApplication>
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
MyLineEdit::MyLineEdit(QWidget *parent) : QLineEdit(parent)
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

		size_t LastPos = 0;
		for (size_t i = 0; i < FileSize; ++i)
		{
			if (Buffer[i] == '\n')
			{
				std::string Temp;
				memcpy(&Temp[0], Buffer + LastPos, i - LastPos - 1);
				Vector.emplace_back(Temp);
				LastPos = i + 1;
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
	ClearTextFormat(this);
	size_t PosPrevious = 0;
	std::string String = Text.toStdString();
	for (size_t i = 0, c = String.size(); i < c; ++i)
	{
		char CurrentChar = String[i];
		if (String[i] == ' ') //Нашли пробел - разделитель
		{
			if (!ExistDictionary(String))
			{
				QList<QTextLayout::FormatRange> Formats;

				QTextCharFormat TextCharFormat;
				TextCharFormat.setUnderlineColor(Qt::red);
				TextCharFormat.setFontUnderline(true);

				QTextLayout::FormatRange TextLayout;
				TextLayout.start = PosPrevious;
				TextLayout.length = i - 1;
				TextLayout.format = TextCharFormat;

				Formats.append(TextLayout);
				SetTextFormat(this, Formats);
				PosPrevious = i + 1;
			}
		}
	}



	/*while (!String.empty())
	{
		size_t StringSize = String.size();
		size_t Pos = String.rfind(' ');
		if (Pos == std::string::npos)
		{
			Vector.emplace_back(std::move(String));
		}
		else
		{
			Vector.emplace_back(String.substr(Pos + 1, StringSize - Pos - 1));
			String.erase(Pos, StringSize - Pos);
		}
	}*/
}
//-----------------------------------------------------------------------------
bool MyLineEdit::ExistDictionary(const std::string &String)
{
	return std::find(Vector.begin(), Vector.end(), String) != Vector.end();
}
//-----------------------------------------------------------------------------
