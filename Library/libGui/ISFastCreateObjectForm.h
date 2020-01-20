#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISInterfaceDialogForm.h"
#include "ISComboEdit.h"
#include "ISPushButton.h"
#include "ISListWidget.h"
//-----------------------------------------------------------------------------
class ISFastCreateRecordsForm : public ISInterfaceDialogForm
{
	Q_OBJECT

public:
	ISFastCreateRecordsForm(QWidget *parent = 0);
	virtual ~ISFastCreateRecordsForm();

protected:
	void Load();
	void Add();
	void Delete();

	void TableChanged(const QVariant &value);

private:
	ISComboEdit *ComboEdit;
	ISPushButton *ButtonAdd;
	ISPushButton *ButtonDelete;
	ISListWidget *ListWidget;
};
//-----------------------------------------------------------------------------
