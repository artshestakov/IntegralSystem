#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISInterfaceForm.h"
//-----------------------------------------------------------------------------
class ISPGSettingsForm : public ISInterfaceForm
{
	Q_OBJECT

public:
	ISPGSettingsForm(QWidget *parent = 0);
	virtual ~ISPGSettingsForm();
};
//-----------------------------------------------------------------------------
