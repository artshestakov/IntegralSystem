#pragma once
//-----------------------------------------------------------------------------
#include "isobjects_global.h"
#include "ISObjectFormBase.h"
//-----------------------------------------------------------------------------
class ISOBJECTS_EXPORT ISCenterSevenPatientsObjectForm : public ISObjectFormBase
{
	Q_OBJECT

public:
	Q_INVOKABLE ISCenterSevenPatientsObjectForm(ISNamespace::ObjectFormType form_type, PMetaClassTable *meta_table, QWidget *parent, int object_id = 0);
	virtual ~ISCenterSevenPatientsObjectForm();

	void SetPhoneMain(const QString &PhoneMain);

protected:
	void AddWaitAdmission();
};
//-----------------------------------------------------------------------------
