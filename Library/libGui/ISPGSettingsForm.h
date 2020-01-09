#pragma once
//-----------------------------------------------------------------------------
#include "libgui_global.h"
#include "ISInterfaceForm.h"
//-----------------------------------------------------------------------------
class LIBGUI_EXPORT ISPGSettingsForm : public ISInterfaceForm
{
	Q_OBJECT

public:
	ISPGSettingsForm(QWidget *parent = 0);
	virtual ~ISPGSettingsForm();
};
//-----------------------------------------------------------------------------
