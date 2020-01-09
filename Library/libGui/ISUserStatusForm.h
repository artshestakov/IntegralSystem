#pragma once
//-----------------------------------------------------------------------------
#include "libgui_global.h"
#include "ISInterfaceDialogForm.h"
//-----------------------------------------------------------------------------
class LIBGUI_EXPORT ISUserStatusForm : public ISInterfaceDialogForm
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
