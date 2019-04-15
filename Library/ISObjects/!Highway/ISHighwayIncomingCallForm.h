#pragma once
//-----------------------------------------------------------------------------
#include "isobjects_global.h"
#include "ISIncomingCallBaseForm.h"
//-----------------------------------------------------------------------------
class ISOBJECTS_EXPORT ISHighwayIncomingCallForm : public ISIncomingCallBaseForm
{
	Q_OBJECT

public:
	Q_INVOKABLE ISHighwayIncomingCallForm(const QVariantMap &payload, QWidget *parent = 0);
	virtual ~ISHighwayIncomingCallForm();

protected:
	void OpenCard() override;

private:
	int OrganizationID;
};
//-----------------------------------------------------------------------------
