#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISIncomingCallBaseForm.h"
//-----------------------------------------------------------------------------
class ISCenterSevenIncomingCallForm : public ISIncomingCallBaseForm
{
	Q_OBJECT

public:
	Q_INVOKABLE ISCenterSevenIncomingCallForm(const QVariantMap &payload, QWidget *parent = 0);
	virtual ~ISCenterSevenIncomingCallForm();

protected:
	void OpenCard() override;

private:
	int PatientID;
	QString PhoneMain;
};
//-----------------------------------------------------------------------------
