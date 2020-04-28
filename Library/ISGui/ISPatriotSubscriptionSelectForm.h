#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISInterfaceDialogForm.h"
//-----------------------------------------------------------------------------
class ISPatriotSubscriptionSelectForm : public ISInterfaceDialogForm
{
	Q_OBJECT

public:
	ISPatriotSubscriptionSelectForm(const QString &Barcode);
	virtual ~ISPatriotSubscriptionSelectForm();

protected:
	void OpenVisit();
	void CloseVisit();

private:
	QButtonGroup *ButtonGroup;
};
//-----------------------------------------------------------------------------