#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISObjectFormBase.h"
//-----------------------------------------------------------------------------
class ISCenterSevenPatientsObjectForm : public ISObjectFormBase
{
	Q_OBJECT

public:
	Q_INVOKABLE ISCenterSevenPatientsObjectForm(ISNamespace::ObjectFormType form_type, PMetaTable *meta_table, QWidget *parent, int object_id = 0);
	virtual ~ISCenterSevenPatientsObjectForm();

	void SetPhoneMain(const QString &PhoneMain);

protected:
	void AddWaitAdmission();
};
//-----------------------------------------------------------------------------
