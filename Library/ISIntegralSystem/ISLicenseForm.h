#pragma once
//-----------------------------------------------------------------------------
#include "isintegralsystem_global.h"
#include "ISInterfaceDialogForm.h"
//-----------------------------------------------------------------------------
class ISINTEGRALSYSTEM_EXPORT ISLicenseForm : public ISInterfaceDialogForm
{
	Q_OBJECT

public:
	ISLicenseForm(QWidget *parent = 0);
	virtual ~ISLicenseForm();
};
//-----------------------------------------------------------------------------
