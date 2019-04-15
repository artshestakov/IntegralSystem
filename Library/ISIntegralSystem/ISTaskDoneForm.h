#pragma once
//-----------------------------------------------------------------------------
#include "isintegralsystem_global.h"
#include "ISInterfaceDialogForm.h"
#include "ISTextEdit.h"
//-----------------------------------------------------------------------------
class ISINTEGRALSYSTEM_EXPORT ISTaskDoneForm : public ISInterfaceDialogForm
{
	Q_OBJECT

public:
	ISTaskDoneForm(int task_id, QWidget *parent = 0);
	virtual ~ISTaskDoneForm();

	void Apply();

private:
	int TaskID;

	QButtonGroup *ButtonGroup;
	ISTextEdit *EditComment;
};
//-----------------------------------------------------------------------------
