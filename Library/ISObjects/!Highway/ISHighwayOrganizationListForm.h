#pragma once
//-----------------------------------------------------------------------------
#include "isobjects_global.h"
#include "ISListBaseForm.h"
//-----------------------------------------------------------------------------
class ISOBJECTS_EXPORT ISHighwayOrganizationListForm : public ISListBaseForm
{
	Q_OBJECT

public:
	Q_INVOKABLE ISHighwayOrganizationListForm(QWidget *parent = 0);
	virtual ~ISHighwayOrganizationListForm();
};
//-----------------------------------------------------------------------------
