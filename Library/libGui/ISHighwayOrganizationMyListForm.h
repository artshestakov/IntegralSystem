#pragma once
//-----------------------------------------------------------------------------
#include "libgui_global.h"
#include "ISListBaseForm.h"
//-----------------------------------------------------------------------------
class LIBGUI_EXPORT ISHighwayOrganizationMyListForm : public ISListBaseForm
{
	Q_OBJECT

public:
	Q_INVOKABLE ISHighwayOrganizationMyListForm(QWidget *parent = 0);
	virtual ~ISHighwayOrganizationMyListForm();
};
//-----------------------------------------------------------------------------
