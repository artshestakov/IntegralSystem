#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISInterfaceForm.h"
//-----------------------------------------------------------------------------
class ISStatisticTablesForm : public ISInterfaceForm
{
	Q_OBJECT

public:
	ISStatisticTablesForm(QWidget *parent = 0);
	virtual ~ISStatisticTablesForm();

protected:
	void DetailsClicked();
};
//-----------------------------------------------------------------------------
