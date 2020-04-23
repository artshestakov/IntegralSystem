#pragma once
//-----------------------------------------------------------------------------
#include "isgui_global.h"
#include "ISInterfaceDialogForm.h"
#include "ISLineEdit.h"
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
	QCheckBox *CheckRememberUser;
	QLabel *LabelCapsLook;
	QLabel *LabelLang;
	QLabel *LabelConnectToDatabase;
	ISServiceButton *ButtonMenu;
	ISWaitWidget *WaitWidget;
	ISPushButton *ButtonInput;
	ISPushButton *ButtonExit;
	ISAuthConnector *AuthConnector;
};
//-----------------------------------------------------------------------------
