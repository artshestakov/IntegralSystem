#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISLineEdit.h"
//-----------------------------------------------------------------------------
class ISTaskNameEdit : public ISLineEdit
{
	Q_OBJECT

public:
	Q_INVOKABLE ISTaskNameEdit(QWidget *parent = 0);
	virtual ~ISTaskNameEdit();

	void Invoke() override;

protected:
	void CreateCompleter(const QStringList &StringList);

private:
	QCompleter *Completer;
	QStringList StringList;
};
//-----------------------------------------------------------------------------
