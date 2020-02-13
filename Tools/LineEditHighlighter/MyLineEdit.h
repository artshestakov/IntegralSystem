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
	ISPoint() : Pos(0), Size(0) { }

	size_t Pos; //������� ������ �����
	size_t Size; //������ �����
};
//-----------------------------------------------------------------------------
struct ISIntArrayItem
{
	int Key;
	int Value;
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
	size_t CountPunct(const char *String, size_t Size);
	ISPoint** ReallocPoints(ISPoint **Points, size_t &PointSize);
	ISPoint* CreatePoint(size_t Start, size_t End);
	QTextLayout::FormatRange CreateFormatRange(int Start, int Lenght);
	void ToLowerString(char *String, size_t Size);
	bool IsDigit(const char *Word, size_t Size); //�������� ����� �� ����� �����
	bool IsDigit(int Char); //�������� ������� �� �����
	bool IsPunct(int Char); //�������� ������� �� ���������������

private:
	char **StringList;
	size_t StringListSize;
};
//-----------------------------------------------------------------------------
#endif
