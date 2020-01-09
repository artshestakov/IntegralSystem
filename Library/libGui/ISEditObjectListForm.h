#pragma once
//-----------------------------------------------------------------------------
#include "libgui_global.h"
#include "ISListObjectForm.h"
//-----------------------------------------------------------------------------
class LIBGUI_EXPORT ISEditObjectListForm : public ISListObjectForm
{
	Q_OBJECT

public:
	Q_INVOKABLE ISEditObjectListForm(PMetaClassTable *MetaTable, int ParentObjectID, QWidget *parent = 0);
	virtual ~ISEditObjectListForm();

protected:
	void LoadDataAfterEvent() override;
};
//-----------------------------------------------------------------------------
