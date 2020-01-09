#pragma once
//-----------------------------------------------------------------------------
#include "libgui_global.h"
#include "ISInterfaceDialogForm.h"
#include "ISNamespace.h"
#include "ISPushButton.h"
//-----------------------------------------------------------------------------
class LIBGUI_EXPORT ISExitForm : public ISInterfaceDialogForm
{
	Q_OBJECT

public:
	ISExitForm(QWidget *parent = 0);
	virtual ~ISExitForm();

	ISNamespace::ExitFormAction GetSelectedAction();

protected:
	void showEvent(QShowEvent *e);
	void ButtonClicked(int ButtonID);
	void Close();

private:
	ISNamespace::ExitFormAction SelectedAction;
	QButtonGroup *ButtonGroup;
};
//-----------------------------------------------------------------------------
