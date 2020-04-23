#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISInterfaceDialogForm.h"
//-----------------------------------------------------------------------------
class ISTableDetailsForm : public ISInterfaceDialogForm
{
	Q_OBJECT

public:
	ISTableDetailsForm(const QString &TableName);
	virtual ~ISTableDetailsForm();
};
//-----------------------------------------------------------------------------
