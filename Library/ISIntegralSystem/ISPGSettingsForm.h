#pragma once
//-----------------------------------------------------------------------------
#include "isintegralsystem_global.h"
#include "ISInterfaceForm.h"
//-----------------------------------------------------------------------------
class ISINTEGRALSYSTEM_EXPORT ISPGSettingsForm : public ISInterfaceForm
{
	Q_OBJECT

public:
	ISPGSettingsForm(QWidget *parent = 0);
	virtual ~ISPGSettingsForm();
};
//-----------------------------------------------------------------------------
