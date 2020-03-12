#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISObjectFormBase.h"
//-----------------------------------------------------------------------------
class ISPatriotSubscriptionObjectForm : public ISObjectFormBase
{
	Q_OBJECT

public:
	Q_INVOKABLE ISPatriotSubscriptionObjectForm(ISNamespace::ObjectFormType form_type, PMetaTable *meta_table, QWidget *parent, int object_id = 0);
	virtual ~ISPatriotSubscriptionObjectForm();

	void SetClient(int ClientID);
	bool Save() override;

protected:
	double GetDiscount(int id) const;
};
//-----------------------------------------------------------------------------
