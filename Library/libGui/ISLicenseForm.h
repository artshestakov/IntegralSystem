#pragma once
//-----------------------------------------------------------------------------
#include "libgui_global.h"
#include "ISInterfaceDialogForm.h"
//-----------------------------------------------------------------------------
class LIBGUI_EXPORT ISLicenseForm : public ISInterfaceDialogForm
{
	Q_OBJECT

public:
	ISLicenseForm(QWidget *parent = 0);
	virtual ~ISLicenseForm();
};
//-----------------------------------------------------------------------------
