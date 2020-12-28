#pragma once
//-----------------------------------------------------------------------------
#include "isgui_global.h"
#include "ISInterfaceDialogForm.h"
#include "ISWaitWidget.h"
#include "ISAuthConnector.h"
#include "ISFieldEdits.h"
//-----------------------------------------------------------------------------
class ISGUI_EXPORT	ISAuthForm : public ISInterfaceDialogForm
{
	Q_OBJECT

public:
	ISAuthForm();
	virtual ~ISAuthForm();

protected:
	void closeEvent(QCloseEvent *CloseEvent);

private:
	void EnterClicked() override;
	void TimeoutCapsLook();

	void ShowConnectionForm(); //Открытие формы настроек подключения
	void ShowAboutForm(); //Открыть форму "О программе"

	void Input(); //Вход в программу
	void InputOld(); //Старый вход в программу (через БД)
	void InputNew(); //Новых вход в программу (через Карат)

	void ConnectedDone();
	void ConnectedFailed();

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
	ISAuthConnector *AuthConnector;
	bool ConnectingState;
};
//-----------------------------------------------------------------------------
