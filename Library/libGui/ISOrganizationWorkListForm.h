#pragma once
//-----------------------------------------------------------------------------
#include "libgui_global.h"
#include "ISListBaseForm.h"
//-----------------------------------------------------------------------------
class LIBGUI_EXPORT ISOrganizationWorkListForm : public ISListBaseForm
{
	Q_OBJECT

public:
	Q_INVOKABLE ISOrganizationWorkListForm(QWidget *parent = 0);
	virtual ~ISOrganizationWorkListForm();

protected:
	void TakeToWork();
};
//-----------------------------------------------------------------------------
