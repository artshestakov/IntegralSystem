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

	void ShowConnectionForm(); //�������� ����� �������� �����������
	void ShowAboutForm(); //������� ����� "� ���������"

	void Input(); //���� � ���������
	void InputOld(); //������ ���� � ��������� (����� ��)
	void InputNew(); //����� ���� � ��������� (����� �����)

	void ConnectedDone();
	void ConnectedFailed();

	void SetConnecting(bool Connecting); //�������� ������� ����������

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
