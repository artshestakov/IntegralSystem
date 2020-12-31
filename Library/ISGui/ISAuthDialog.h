#pragma once
//-----------------------------------------------------------------------------
#include "isgui_global.h"
#include "ISInterfaceDialogForm.h"
#include "ISWaitWidget.h"
#include "ISFieldEdits.h"
//-----------------------------------------------------------------------------
class ISGUI_EXPORT	ISAuthDialog : public ISInterfaceDialogForm
{
	Q_OBJECT

public:
	ISAuthDialog();
	virtual ~ISAuthDialog();

protected:
	void closeEvent(QCloseEvent *CloseEvent);
	void EnterClicked() override;

private:
	void ShowConnectionForm(); //Открытие формы настроек подключения
	void ShowAboutForm(); //Открыть форму "О программе"

	void Input(); //Вход в программу
	void SetConnecting(bool Connecting); //Изменить рисовку интерфейса

private:
	ISLineEdit *EditLogin;
	ISLineEdit *EditPassword;
	ISCheckEdit *CheckRememberUser;
	QLabel *LabelIndicator;
	ISServiceButton *ButtonMenu;
	ISWaitWidget *WaitWidget;
	ISPushButton *ButtonInput;
	ISPushButton *ButtonExit;
	bool ConnectingState;
};
//-----------------------------------------------------------------------------
