#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISInterfaceDialogForm.h"
#include "ISFieldEdits.h"
//-----------------------------------------------------------------------------
class ISConnectionForm : public ISInterfaceDialogForm
{
	Q_OBJECT

public:
	ISConnectionForm();
	virtual ~ISConnectionForm();

protected:
	void AfterShowEvent() override;
	void EnterClicked() override;

private:
	void SaveSettings(); //Программное сохранение настроек
	bool CheckFields(); //Проверка на заполнение полей

private:
	ISLineEdit *EditServer;
	ISIntegerEdit *EditPort;
	ISLineEdit *EditDatabase;

	ISCheckEdit *EditProtocolUse;
	ISCheckEdit *EditProtocolAuth;
	ISLineEdit *EditProtocolHost;
	ISIntegerEdit *EditProtocolPort;
};
//-----------------------------------------------------------------------------
