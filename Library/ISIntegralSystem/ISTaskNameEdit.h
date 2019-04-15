#pragma once
//-----------------------------------------------------------------------------
#include "isintegralsystem_global.h"
#include "ISLineEdit.h"
//-----------------------------------------------------------------------------
class ISINTEGRALSYSTEM_EXPORT ISTaskNameEdit : public ISLineEdit
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
