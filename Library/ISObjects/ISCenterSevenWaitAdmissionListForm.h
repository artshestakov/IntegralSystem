#pragma once
//-----------------------------------------------------------------------------
#include "isobjects_global.h"
#include "ISListBaseForm.h"
//-----------------------------------------------------------------------------
class ISOBJECTS_EXPORT ISCenterSevenWaitAdmissionListForm : public ISListBaseForm
{
	Q_OBJECT

public:
	Q_INVOKABLE ISCenterSevenWaitAdmissionListForm(QWidget *parent = 0);
	virtual ~ISCenterSevenWaitAdmissionListForm();

protected:
	void CloseWait();
};
//-----------------------------------------------------------------------------
