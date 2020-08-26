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
	void AfterShowEvent() override;
	void EnterClicked() override;
	void TimeoutCapsLook();
	void TimeoutLang();

	void ShowConnectionForm(); //Открытие формы настроек подключения
	void ShowAboutForm(); //Открыть форму "О программе"

	void Input(); //Вход в программу
	void ConnectedDone();
	void ConnectedFailed();

	void SetConnecting(bool Connecting); //Изменить рисовку интерфейса
	bool Check(); //Проверки
	void ClearFields();

private:
	ISLineEdit *EditLogin;
	ISPasswordEdit *EditPassword;
	ISCheckEdit *CheckRememberUser;
	QLabel *LabelCapsLook;
	QLabel *LabelLang;
	QLabel *LabelConnectToDatabase;
	ISServiceButton *ButtonMenu;
	ISWaitWidget *WaitWidget;
	ISPushButton *ButtonInput;
	ISPushButton *ButtonExit;
	ISAuthConnector *AuthConnector;
	bool ConnectingState;
	QTimer *TimerCapsLook;
	QTimer *TimerLang;
};
//-----------------------------------------------------------------------------
