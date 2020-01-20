#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISInterfaceForm.h"
#include "ISListWidget.h"
//-----------------------------------------------------------------------------
class ISDeviceSettingsForm : public ISInterfaceForm
{
	Q_OBJECT

public:
	ISDeviceSettingsForm(QWidget *parent = 0);
	virtual ~ISDeviceSettingsForm();

protected:
	void ItemSelectionChanged();
	void ItemDoubleClicked(QListWidgetItem *ListWidgetItem);
	void Reload();

	void Create();
	void Edit();
	void Delete();
	void Update();

private:
	QAction *ActionCreate;
	QAction *ActionEdit;
	QAction *ActionDelete;
	QAction *ActionUpdate;

	ISListWidget *ListWidget;
};
//-----------------------------------------------------------------------------
