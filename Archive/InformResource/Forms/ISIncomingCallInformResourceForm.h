#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISIncomingCallBaseForm.h"
//-----------------------------------------------------------------------------
class ISIncomingCallInformResourceForm : public ISIncomingCallBaseForm
{
	Q_OBJECT

public:
	Q_INVOKABLE ISIncomingCallInformResourceForm(const QVariantMap &payload);
	virtual ~ISIncomingCallInformResourceForm();

protected:
	void OpenCard() override;

private:
	int OrganizationID;
};
//-----------------------------------------------------------------------------
