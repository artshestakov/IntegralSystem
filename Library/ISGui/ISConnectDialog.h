#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISInterfaceDialogForm.h"
#include "ISFieldEdits.h"
//-----------------------------------------------------------------------------
class ISConnectDialog : public ISInterfaceDialogForm
{
	Q_OBJECT

public:
	ISConnectDialog();
	virtual ~ISConnectDialog();

protected:
	void AfterShowEvent() override;
	void EnterClicked() override;

private:
	void Save(); //Программное сохранение настроек

private:
	ISLineEdit *EditServer;
	ISIntegerEdit *EditPort;
};
//-----------------------------------------------------------------------------
