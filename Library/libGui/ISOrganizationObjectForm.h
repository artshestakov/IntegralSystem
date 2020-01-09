#pragma once
//-----------------------------------------------------------------------------
#include "libgui_global.h"
#include "ISObjectFormBase.h"
#include "ISINNEdit.h"
#include "ISDaDataService.h"
//-----------------------------------------------------------------------------
class LIBGUI_EXPORT ISOrganizationObjectForm : public ISObjectFormBase
{
	Q_OBJECT

public:
	Q_INVOKABLE ISOrganizationObjectForm(ISNamespace::ObjectFormType form_type, PMetaClassTable *meta_table, QWidget *parent, int object_id = 0);
	virtual ~ISOrganizationObjectForm();

protected:
	void INNChanged(const QVariant &value);
	void Notify();
	void DemoAccess();
	void Score();
	void Called();
	void UpdateDemo();

	void SearchFromINN();
	void SearchFinished();

private:
	QAction *ActionDemo;
	QAction *ActionScore;

	ISINNEdit *INNEdit;
	ISDaDataService *DaDataService;
};
//-----------------------------------------------------------------------------
