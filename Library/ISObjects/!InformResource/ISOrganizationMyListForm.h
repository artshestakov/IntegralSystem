#pragma once
//-----------------------------------------------------------------------------
#include "isobjects_global.h"
#include "ISListBaseForm.h"
//-----------------------------------------------------------------------------
class ISOBJECTS_EXPORT ISOrganizationMyListForm : public ISListBaseForm
{
	Q_OBJECT

public:
	Q_INVOKABLE ISOrganizationMyListForm(QWidget *parent = 0);
	virtual ~ISOrganizationMyListForm();

protected:
	void Return();
};
//-----------------------------------------------------------------------------
