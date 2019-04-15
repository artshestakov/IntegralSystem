#pragma once
//-----------------------------------------------------------------------------
#include "isintegralsystem_global.h"
#include "ISInterfaceDialogForm.h"
#include "ISLineEdit.h"
#include "ISPasswordEdit.h"
#include "ISWaitWidget.h"
#include "ISAuthConnector.h"
//-----------------------------------------------------------------------------
class ISINTEGRALSYSTEM_EXPORT ISAuthorizationForm : public ISInterfaceDialogForm
{
	Q_OBJECT

public:
	ISAuthorizationForm(QWidget *parent = 0);
	virtual ~ISAuthorizationForm();
	
	QString GetEnteredLogin() const; //�������� ��������� �����
	QString GetEnteredPassword() const; //�������� ��������� ������

protected:
	void AfterShowEvent() override;
	void EnterClicked() override;
	void TimeoutCapsLook();
	void TimeoutLang();

	void ShowConnectionForm(); //�������� ����� �������� �����������
	void ShowAboutForm(); //������� ����� "� ���������"

	void Input(); //���� � ���������
	void ConnectedDone();
	void ConnectedFailed();

	void SetConnecting(bool Connecting); //�������� ������� ����������
	bool Check(); //��������
	void ClearFields();

private:
	ISLineEdit *EditLogin;
	ISPasswordEdit *EditPassword;
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
