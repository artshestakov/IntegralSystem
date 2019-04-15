#pragma once
//-----------------------------------------------------------------------------
#include "isintegralsystem_global.h"
#include "ISInterfaceMetaForm.h"
//-----------------------------------------------------------------------------
class ISINTEGRALSYSTEM_EXPORT ISControlDatabaseForm : public ISInterfaceMetaForm
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

	void ShowDatabaseSettings();

private:
	QTabWidget *TabWidget;
};
//-----------------------------------------------------------------------------
