#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISIncomingCallBaseForm.h"
//-----------------------------------------------------------------------------
class ISHighwayIncomingCallForm : public ISIncomingCallBaseForm
{
	Q_OBJECT

public:
	Q_INVOKABLE ISHighwayIncomingCallForm(const QVariantMap &payload);
	virtual ~ISHighwayIncomingCallForm();

protected:
	void OpenCard() override;

private:
	int OrganizationID;
};
//-----------------------------------------------------------------------------
