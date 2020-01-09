#pragma once
//-----------------------------------------------------------------------------
#include "libgui_global.h"
#include "ISListBaseForm.h"
//-----------------------------------------------------------------------------
class LIBGUI_EXPORT ISHighwayBillsListForm : public ISListBaseForm
{
	Q_OBJECT

public:
	Q_INVOKABLE ISHighwayBillsListForm(QWidget *parent = 0);
	virtual ~ISHighwayBillsListForm();
};
//-----------------------------------------------------------------------------
