#pragma once
//-----------------------------------------------------------------------------
#include "isintegralsystem_global.h"
#include "ISListObjectForm.h"
//-----------------------------------------------------------------------------
class ISINTEGRALSYSTEM_EXPORT ISEditObjectListForm : public ISListObjectForm
{
	Q_OBJECT

public:
	Q_INVOKABLE ISEditObjectListForm(PMetaClassTable *MetaTable, int ParentObjectID, QWidget *parent = 0);
	virtual ~ISEditObjectListForm();

protected:
	void LoadDataAfterEvent() override;
};
//-----------------------------------------------------------------------------
