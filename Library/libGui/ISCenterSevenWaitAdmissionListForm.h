#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISListBaseForm.h"
//-----------------------------------------------------------------------------
class ISCenterSevenWaitAdmissionListForm : public ISListBaseForm
{
	Q_OBJECT

public:
	Q_INVOKABLE ISCenterSevenWaitAdmissionListForm(QWidget *parent = 0);
	virtual ~ISCenterSevenWaitAdmissionListForm();

protected:
	void CloseWait();
};
//-----------------------------------------------------------------------------
