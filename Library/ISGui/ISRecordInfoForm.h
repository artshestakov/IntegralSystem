#pragma once
//-----------------------------------------------------------------------------
#include "isgui_global.h"
#include "ISInterfaceDialogForm.h"
#include "PMetaClassTable.h"
//-----------------------------------------------------------------------------
class ISGUI_EXPORT ISRecordInfoForm : public ISInterfaceDialogForm
{
	Q_OBJECT

public:
	ISRecordInfoForm(PMetaClassTable *MetaTable, int ObjectID, QWidget *parent = 0);
	virtual ~ISRecordInfoForm();
};
//-----------------------------------------------------------------------------
