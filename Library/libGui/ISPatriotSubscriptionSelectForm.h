#pragma once
//-----------------------------------------------------------------------------
#include "libgui_global.h"
#include "ISInterfaceDialogForm.h"
//-----------------------------------------------------------------------------
class LIBGUI_EXPORT ISPatriotSubscriptionSelectForm : public ISInterfaceDialogForm
{
	Q_OBJECT

public:
	ISPatriotSubscriptionSelectForm(const QString &Barcode, QWidget *parent = 0);
	virtual ~ISPatriotSubscriptionSelectForm();

protected:
	void OpenVisit();
	void CloseVisit();

private:
	QButtonGroup *ButtonGroup;
};
//-----------------------------------------------------------------------------
