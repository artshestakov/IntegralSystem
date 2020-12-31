#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISInterfaceDialogForm.h"
#include "ISFieldEdits.h"
//-----------------------------------------------------------------------------
class ISConnectSettingDialog : public ISInterfaceDialogForm
{
	Q_OBJECT

public:
	ISConnectSettingDialog();
	virtual ~ISConnectSettingDialog();

protected:
	void AfterShowEvent() override;
	void EnterClicked() override;

private:
	void Save(); //Программное сохранение настроек

private:
	ISLineEdit *EditServer;
	ISIntegerEdit *EditPort;
	ISLineEdit *EditDatabase;
	ISCheckEdit *EditProtocolInclude;
	ISIntegerEdit *EditProtocolPort;
};
//-----------------------------------------------------------------------------
