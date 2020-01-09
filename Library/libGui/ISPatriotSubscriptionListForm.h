#pragma once
//-----------------------------------------------------------------------------
#include "libgui_global.h"
#include "ISListBaseForm.h"
//-----------------------------------------------------------------------------
class LIBGUI_EXPORT ISPatriotSubscriptionListForm : public ISListBaseForm
{
	Q_OBJECT

public:
	Q_INVOKABLE ISPatriotSubscriptionListForm(QWidget *parent = 0);
	virtual ~ISPatriotSubscriptionListForm();

protected:
	void Register();
};
//-----------------------------------------------------------------------------
