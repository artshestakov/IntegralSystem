#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISListBaseForm.h"
//-----------------------------------------------------------------------------
class ISOrganizationListForm : public ISListBaseForm
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
