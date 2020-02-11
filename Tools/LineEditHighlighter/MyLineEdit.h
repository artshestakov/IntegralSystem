#pragma once
#ifndef _ISMETAUSER_H_INCLUDED
#define _ISMETAUSER_H_INCLUDED
//-----------------------------------------------------------------------------
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QWidget>
#include <string>
//-----------------------------------------------------------------------------
class MyLineEdit : public QLineEdit
{
	Q_OBJECT

public:
	MyLineEdit(QWidget *parent = 0);
	virtual ~MyLineEdit();

private:
	void TextChanged(const QString &Text);
	bool ExistDictionary(const std::string &String);

private:
	std::vector<std::string> Vector;
};
//-----------------------------------------------------------------------------
#endif
