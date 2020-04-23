#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISListBaseForm.h"
//-----------------------------------------------------------------------------
class ISOrganizationWorkListForm : public ISListBaseForm
{
	Q_OBJECT

public:
	Q_INVOKABLE ISOrganizationWorkListForm(QWidget *parent = 0);
	virtual ~ISOrganizationWorkListForm();

protected:
	void TakeToWork();
};
//-----------------------------------------------------------------------------
