#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISListBaseForm.h"
//-----------------------------------------------------------------------------
class ISHighwayOrganizationListForm : public ISListBaseForm
{
	Q_OBJECT

public:
	Q_INVOKABLE ISHighwayOrganizationListForm(QWidget *parent = 0);
	virtual ~ISHighwayOrganizationListForm();
};
//-----------------------------------------------------------------------------
