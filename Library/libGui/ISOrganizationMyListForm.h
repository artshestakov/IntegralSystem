#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISListBaseForm.h"
//-----------------------------------------------------------------------------
class ISOrganizationMyListForm : public ISListBaseForm
{
	Q_OBJECT

public:
	Q_INVOKABLE ISOrganizationMyListForm(QWidget *parent = 0);
	virtual ~ISOrganizationMyListForm();

protected:
	void Return();
};
//-----------------------------------------------------------------------------
