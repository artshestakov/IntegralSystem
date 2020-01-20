#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISListBaseForm.h"
//-----------------------------------------------------------------------------
class ISHighwayBillsListForm : public ISListBaseForm
{
	Q_OBJECT

public:
	Q_INVOKABLE ISHighwayBillsListForm(QWidget *parent = 0);
	virtual ~ISHighwayBillsListForm();
};
//-----------------------------------------------------------------------------
