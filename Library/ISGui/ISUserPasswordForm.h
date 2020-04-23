#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISInterfaceDialogForm.h"
#include "ISLineEdit.h"
#include "ISButtonDialog.h"
//-----------------------------------------------------------------------------
class ISUserPasswordForm : public ISInterfaceDialogForm
{
	Q_OBJECT

public:
	ISUserPasswordForm(int user_id);
	virtual ~ISUserPasswordForm();

protected:
	void AfterShowEvent() override;
	void EnterClicked() override;
	void ChangePassword();
	void PasswordChecked();
	void SetIconAndTextHeader(const QIcon &Icon, const QString &Text);

private:
	QLabel *LabelIcon;
	QLabel *LabelText;

	ISLineEdit *EditCurrentPassword;
	ISLineEdit *EditPassword;
	ISLineEdit *EditPasswordCheck;
	ISButtonDialog *ButtonDialog;

	int UserID;
	QString Login;
	QString UserFullName;
};
//-----------------------------------------------------------------------------
