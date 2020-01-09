#pragma once
//-----------------------------------------------------------------------------
#include "libgui_global.h"
#include "ISListBaseForm.h"
//-----------------------------------------------------------------------------
class LIBGUI_EXPORT ISHighwayOrganizationListForm : public ISListBaseForm
{
	Q_OBJECT

public:
	Q_INVOKABLE ISHighwayOrganizationListForm(QWidget *parent = 0);
	virtual ~ISHighwayOrganizationListForm();
};
//-----------------------------------------------------------------------------
