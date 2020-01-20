#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISInterfaceDialogForm.h"
#include "PMetaClassTable.h"
//-----------------------------------------------------------------------------
class ISRecordInfoForm : public ISInterfaceDialogForm
{
	Q_OBJECT

public:
	ISRecordInfoForm(PMetaClassTable *MetaTable, int ObjectID, QWidget *parent = 0);
	virtual ~ISRecordInfoForm();
};
//-----------------------------------------------------------------------------
