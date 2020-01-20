#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISObjectFormBase.h"
//-----------------------------------------------------------------------------
class ISCenterSevenAdmissionObjectForm : public ISObjectFormBase
{
	Q_OBJECT

public:
	Q_INVOKABLE ISCenterSevenAdmissionObjectForm(ISNamespace::ObjectFormType form_type, PMetaClassTable *meta_table, QWidget *parent, int object_id = 0);
	virtual ~ISCenterSevenAdmissionObjectForm();

	void SetDate(const QDate &date);
	void SetDoctor(int doctor_id);

protected:
	bool Save() override;

private:
	QDate Date;
	int StatusID;
};
//-----------------------------------------------------------------------------
