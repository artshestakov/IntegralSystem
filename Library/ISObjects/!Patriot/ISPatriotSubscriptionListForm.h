#pragma once
//-----------------------------------------------------------------------------
#include "isobjects_global.h"
#include "ISListBaseForm.h"
//-----------------------------------------------------------------------------
class ISOBJECTS_EXPORT ISPatriotSubscriptionListForm : public ISListBaseForm
{
	Q_OBJECT

public:
	Q_INVOKABLE ISPatriotSubscriptionListForm(QWidget *parent = 0);
	virtual ~ISPatriotSubscriptionListForm();

protected:
	void Register();
};
//-----------------------------------------------------------------------------
