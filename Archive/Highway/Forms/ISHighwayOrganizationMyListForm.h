#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISListBaseForm.h"
//-----------------------------------------------------------------------------
class ISHighwayOrganizationMyListForm : public ISListBaseForm
{
	Q_OBJECT

public:
	Q_INVOKABLE ISHighwayOrganizationMyListForm(QWidget *parent = 0);
	virtual ~ISHighwayOrganizationMyListForm();
};
//-----------------------------------------------------------------------------
