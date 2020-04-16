#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISInterfaceDialogForm.h"
#include "ISTextEdit.h"
//-----------------------------------------------------------------------------
class ISTaskDoneForm : public ISInterfaceDialogForm
{
	Q_OBJECT

public:
	ISTaskDoneForm(int task_id);
	virtual ~ISTaskDoneForm();

	void Apply();

private:
	int TaskID;

	QButtonGroup *ButtonGroup;
	ISTextEdit *EditComment;
};
//-----------------------------------------------------------------------------
