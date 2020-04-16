#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISInterfaceDialogForm.h"
//-----------------------------------------------------------------------------
class ISAttachFileBaseForm : public ISInterfaceDialogForm
{
	Q_OBJECT

signals:
	void UpdateList();

public:
	ISAttachFileBaseForm();
	virtual ~ISAttachFileBaseForm();
};
//-----------------------------------------------------------------------------
