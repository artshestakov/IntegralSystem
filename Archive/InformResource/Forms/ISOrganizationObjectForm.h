#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISObjectFormBase.h"
#include "ISFieldEdits.h"
#include "ISDaDataService.h"
//-----------------------------------------------------------------------------
class ISOrganizationObjectForm : public ISObjectFormBase
{
	Q_OBJECT

public:
	Q_INVOKABLE ISOrganizationObjectForm(ISNamespace::ObjectFormType form_type, PMetaTable *meta_table, QWidget *parent, int object_id = 0);
	virtual ~ISOrganizationObjectForm();

protected:
	void INNChanged(const QVariant &value);
	void Notify();
	void DemoAccess();
	void Score();
	void Called();
	void UpdateDemo();

	void SearchFromINN();
	void SearchFinished(const ISDaDataOrganization &OrganizationStruct);

private:
	QAction *ActionDemo;
	QAction *ActionScore;

	ISINNEdit *INNEdit;
	ISDaDataService *DaDataService;
};
//-----------------------------------------------------------------------------
