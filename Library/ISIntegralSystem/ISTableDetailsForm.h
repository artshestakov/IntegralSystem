#pragma once
//-----------------------------------------------------------------------------
#include "isintegralsystem_global.h"
#include "ISInterfaceDialogForm.h"
//-----------------------------------------------------------------------------
class ISINTEGRALSYSTEM_EXPORT ISTableDetailsForm : public ISInterfaceDialogForm
{
	Q_OBJECT

public:
	ISTableDetailsForm(const QString &TableName, QWidget *parent = 0);
	virtual ~ISTableDetailsForm();
};
//-----------------------------------------------------------------------------
