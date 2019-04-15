#pragma once
//-----------------------------------------------------------------------------
#include "isintegralsystem_global.h"
#include "ISInterfaceDialogForm.h"
//-----------------------------------------------------------------------------
class ISINTEGRALSYSTEM_EXPORT ISUserStatusForm : public ISInterfaceDialogForm
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
