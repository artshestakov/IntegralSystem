#pragma once
//-----------------------------------------------------------------------------
#include "libgui_global.h"
#include "ISInterfaceDialogForm.h"
//-----------------------------------------------------------------------------
class LIBGUI_EXPORT ISTableDetailsForm : public ISInterfaceDialogForm
{
	Q_OBJECT

public:
	ISTableDetailsForm(const QString &TableName, QWidget *parent = 0);
	virtual ~ISTableDetailsForm();
};
//-----------------------------------------------------------------------------
