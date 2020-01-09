#pragma once
//-----------------------------------------------------------------------------
#include "libgui_global.h"
#include "ISInterfaceDialogForm.h"
#include "ISPathEditFile.h"
#include "ISLineEdit.h"
#include "ISButtonDialog.h"
//-----------------------------------------------------------------------------
class LIBGUI_EXPORT ISTaskFileInsertForm : public ISInterfaceDialogForm
{
	Q_OBJECT

public:
	ISTaskFileInsertForm(int task_id, QWidget *parent = 0);
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
