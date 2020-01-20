#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISInterfaceDialogForm.h"
#include "ISTextEdit.h"
//-----------------------------------------------------------------------------
class ISNoteObjectForm : public ISInterfaceDialogForm
{
	Q_OBJECT

public:
	ISNoteObjectForm(const QString &table_name, int object_id, QWidget *parent = 0);
	virtual ~ISNoteObjectForm();

protected:
	void Save();
	void LoadNote();

private:
	QString TableName;
	int ObjectID;

	ISTextEdit *TextEdit;
};
//-----------------------------------------------------------------------------
