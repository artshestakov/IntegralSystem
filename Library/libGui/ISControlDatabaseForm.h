#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISInterfaceMetaForm.h"
//-----------------------------------------------------------------------------
class ISControlDatabaseForm : public ISInterfaceMetaForm
{
	Q_OBJECT

public:
	Q_INVOKABLE ISControlDatabaseForm(QWidget *parent = 0);
	virtual ~ISControlDatabaseForm();

	void LoadData() override;

protected:
	void CreateGeneralTab();
	void CreatePGSettings();
	void CreateStatisticTablesForm();
	void CreateDistFilesForm();

private:
	QTabWidget *TabWidget;
};
//-----------------------------------------------------------------------------
