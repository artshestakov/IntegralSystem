#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISInterfaceDialogForm.h"
#include "ISButtons.h"
#include "ISLineEdit.h"
#include "ISButtonDialog.h"
#include "ISListWidget.h"
#include "ISFieldEdits.h"
//-----------------------------------------------------------------------------
class ISExternalToolsForm : public ISInterfaceDialogForm
{
	Q_OBJECT

public:
	ISExternalToolsForm();
	virtual ~ISExternalToolsForm();

protected:
	void Load();
	void Save();

	void Add();
	void Delete();
	void Up();
	void Down();

	void CountItemsChanged();
	void CurrentItemChanged(QListWidgetItem *CurrentItem, QListWidgetItem *PreviousItem);
	void NameChanged(const QVariant &Name);
	void CommandChanged(const QVariant &Command);

private:
	ISListWidget *ListWidget;
	ISPushButton *ButtonAdd;
	ISPushButton *ButtonDelete;
	ISPushButton *ButtonUp;
	ISPushButton *ButtonDown;

	ISLineEdit *EditName;
	ISPathEditFile *EditCommand;

	ISButtonDialog *ButtonDialog;
};
//-----------------------------------------------------------------------------
