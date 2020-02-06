#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISInterfaceDialogForm.h"
#include "ISNamespace.h"

//-----------------------------------------------------------------------------
class ISExitForm : public ISInterfaceDialogForm
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
