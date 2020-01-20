#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISInterfaceDialogForm.h"
//-----------------------------------------------------------------------------
class ISUserStatusForm : public ISInterfaceDialogForm
{
	Q_OBJECT

public:
	ISUserStatusForm(QWidget *parent = 0);
	virtual ~ISUserStatusForm();

protected:
	void StatusChange();
	void StatusClear();
	void Close();
};
//-----------------------------------------------------------------------------
