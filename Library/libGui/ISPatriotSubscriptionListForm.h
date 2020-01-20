#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISListBaseForm.h"
//-----------------------------------------------------------------------------
class ISPatriotSubscriptionListForm : public ISListBaseForm
{
	Q_OBJECT

public:
	Q_INVOKABLE ISPatriotSubscriptionListForm(QWidget *parent = 0);
	virtual ~ISPatriotSubscriptionListForm();

protected:
	void Register();
};
//-----------------------------------------------------------------------------
