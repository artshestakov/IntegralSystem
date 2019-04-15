#pragma once
//-----------------------------------------------------------------------------
#include "isobjects_global.h"
#include "ISListBaseForm.h"
//-----------------------------------------------------------------------------
class ISOBJECTS_EXPORT ISHighwayOrganizationMyListForm : public ISListBaseForm
{
	Q_OBJECT

public:
	Q_INVOKABLE ISHighwayOrganizationMyListForm(QWidget *parent = 0);
	virtual ~ISHighwayOrganizationMyListForm();
};
//-----------------------------------------------------------------------------
