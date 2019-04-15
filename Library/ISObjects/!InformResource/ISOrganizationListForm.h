#pragma once
//-----------------------------------------------------------------------------
#include "isobjects_global.h"
#include "ISListBaseForm.h"
//-----------------------------------------------------------------------------
class ISOBJECTS_EXPORT ISOrganizationListForm : public ISListBaseForm
{
	Q_OBJECT

public:
	Q_INVOKABLE ISOrganizationListForm(QWidget *parent = 0);
	virtual ~ISOrganizationListForm();

protected:
	void UserChanged(const QVariant &value);
	void Transfer();
	void ResetExecutor();
	void Percentage();
	void CalledMoreDays();
};
//-----------------------------------------------------------------------------
