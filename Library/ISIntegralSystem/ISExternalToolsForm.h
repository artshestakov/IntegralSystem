#pragma once
//-----------------------------------------------------------------------------
#include "isintegralsystem_global.h"
#include "ISInterfaceDialogForm.h"
#include "ISPushButton.h"
#include "ISLineEdit.h"
#include "ISPathEditFile.h"
#include "ISButtonDialog.h"
#include "ISListWidget.h"
//-----------------------------------------------------------------------------
class ISINTEGRALSYSTEM_EXPORT ISExternalToolsForm : public ISInterfaceDialogForm
{
	Q_OBJECT

public:
	ISExternalToolsForm(QWidget *parent = 0);
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
