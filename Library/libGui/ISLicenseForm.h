#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISInterfaceDialogForm.h"
//-----------------------------------------------------------------------------
class ISLicenseForm : public ISInterfaceDialogForm
{
	Q_OBJECT

public:
	ISLicenseForm(QWidget *parent = 0);
	virtual ~ISLicenseForm();
};
//-----------------------------------------------------------------------------
