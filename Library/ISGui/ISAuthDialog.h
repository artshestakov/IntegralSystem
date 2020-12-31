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
	void ShowConnectionForm(); //�������� ����� �������� �����������
	void ShowAboutForm(); //������� ����� "� ���������"

	void Input(); //���� � ���������
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
	bool ConnectingState;
};
//-----------------------------------------------------------------------------
