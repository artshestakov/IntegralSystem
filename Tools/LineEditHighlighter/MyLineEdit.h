#pragma once
#ifndef _ISMETAUSER_H_INCLUDED
#define _ISMETAUSER_H_INCLUDED
//-----------------------------------------------------------------------------
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QWidget>
#include <QtGui/QTextLayout>
//-----------------------------------------------------------------------------
struct ISPoint
{
	ISPoint() : Start(0), End(0) { }

	size_t Start;
	size_t End;
};
//-----------------------------------------------------------------------------
class MyLineEdit : public QLineEdit
{
	Q_OBJECT

public:
	MyLineEdit(QWidget *parent = 0);
	virtual ~MyLineEdit();

private:
	void TextChanged(const QString &Text);
	bool ExistDictionary(const char *String);
	size_t CountSpace(const char *String, size_t Size);
	ISPoint* CreatePoint(size_t Start, size_t End);
	QTextLayout::FormatRange CreateFormatRange(int Start, int Lenght);
	void ToLowerString(char *String, size_t Size);
	bool IsDigit(const char *Word, size_t Size);

private:
	char **StringList;
	size_t StringListSize;
};
//-----------------------------------------------------------------------------
#endif
