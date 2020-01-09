#pragma once
//-----------------------------------------------------------------------------
#include "libgui_global.h"
#include "ISObjectFormBase.h"
//-----------------------------------------------------------------------------
class LIBGUI_EXPORT ISCallHistoryObjectForm : public ISObjectFormBase
{
	Q_OBJECT

public:
	Q_INVOKABLE ISCallHistoryObjectForm(ISNamespace::ObjectFormType form_type, PMetaClassTable *meta_table, QWidget *parent, int object_id = 0);
	virtual ~ISCallHistoryObjectForm();
};
//-----------------------------------------------------------------------------
