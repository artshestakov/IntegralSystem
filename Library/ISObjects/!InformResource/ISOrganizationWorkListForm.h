#pragma once
//-----------------------------------------------------------------------------
#include "isobjects_global.h"
#include "ISListBaseForm.h"
//-----------------------------------------------------------------------------
class ISOBJECTS_EXPORT ISOrganizationWorkListForm : public ISListBaseForm
{
	Q_OBJECT

public:
	Q_INVOKABLE ISOrganizationWorkListForm(QWidget *parent = 0);
	virtual ~ISOrganizationWorkListForm();

protected:
	void TakeToWork();
};
//-----------------------------------------------------------------------------
