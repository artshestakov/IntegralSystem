#pragma once
//-----------------------------------------------------------------------------
#include "isobjects_global.h"
#include "ISListBaseForm.h"
//-----------------------------------------------------------------------------
class ISOBJECTS_EXPORT ISHighwayBillsListForm : public ISListBaseForm
{
	Q_OBJECT

public:
	Q_INVOKABLE ISHighwayBillsListForm(QWidget *parent = 0);
	virtual ~ISHighwayBillsListForm();
};
//-----------------------------------------------------------------------------
