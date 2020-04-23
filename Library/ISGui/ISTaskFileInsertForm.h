#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISInterfaceDialogForm.h"
#include "ISLineEdit.h"
#include "ISButtonDialog.h"
#include "ISFieldEdits.h"
//-----------------------------------------------------------------------------
class ISTaskFileInsertForm : public ISInterfaceDialogForm
{
	Q_OBJECT

public:
	ISTaskFileInsertForm(int task_id);
	virtual ~ISTaskFileInsertForm();

	QString GetFileName() const;

protected:
	void EnterClicked() override;
	void PathEditfileChanged();
	void Insert();

private:
	int TaskID;

	ISPathEditFile *PathEditFile;
	ISLineEdit *EditNote;
	QProgressBar *ProgressBar;
	ISButtonDialog *ButtonDialog;
};
//-----------------------------------------------------------------------------
