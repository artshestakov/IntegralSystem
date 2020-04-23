#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISInterfaceDialogForm.h"
#include "PMetaClass.h"
//-----------------------------------------------------------------------------
class ISRecordInfoForm : public ISInterfaceDialogForm
{
	Q_OBJECT

public:
	ISRecordInfoForm(PMetaTable *MetaTable, int ObjectID);
	virtual ~ISRecordInfoForm();
};
//-----------------------------------------------------------------------------
