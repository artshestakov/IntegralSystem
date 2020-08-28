#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISInterfaceMetaForm.h"
#include "ISFieldEdits.h"
//-----------------------------------------------------------------------------
class ISControlDatabaseForm : public ISInterfaceMetaForm
{
	Q_OBJECT

public:
	Q_INVOKABLE ISControlDatabaseForm(QWidget *parent = 0);
	virtual ~ISControlDatabaseForm();

	void LoadData() override;

private:
	void CreateGeneralTab();
	void CreatePGSettings();
	void CreateStatisticTablesForm();
	void CreateSettingLocalNameFields();

private:
	void DetailsClicked();

private:
	QTabWidget *TabWidget;

private:
	std::vector<ISLineEdit*> Edits;
};
//-----------------------------------------------------------------------------
