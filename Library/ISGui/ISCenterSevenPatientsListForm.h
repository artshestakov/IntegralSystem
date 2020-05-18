#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISListBaseForm.h"
//-----------------------------------------------------------------------------
class ISCenterSevenPatientsListForm : public ISListBaseForm
{
	Q_OBJECT

public:
	Q_INVOKABLE ISCenterSevenPatientsListForm(QWidget *parent = 0);
	virtual ~ISCenterSevenPatientsListForm();

protected:
	void AddWaitAdmission();
};
//-----------------------------------------------------------------------------
