#pragma once
#ifndef _ISUSERHASHFORM_H_INCLUDED
#define _ISUSERHASHFORM_H_INCLUDED
//-----------------------------------------------------------------------------
#include "ISInterfaceDialogForm.h"
#include "ISFieldEdits.h"
//-----------------------------------------------------------------------------
class ISUserPasswordDialog : public ISInterfaceDialogForm
{
	Q_OBJECT

public:
	ISUserPasswordDialog(unsigned int user_id, const QString &user_fio, const QString &user_login);
	virtual ~ISUserPasswordDialog();

protected:
	void AfterShowEvent() override;
	void EnterClicked() override;

private:
	void Apply();

private:
	ISLineEdit *EditPasswordCurrent;
	ISLineEdit *EditPassword;
	ISLineEdit *EditPasswordCheck;
	QLabel *LabelText;
	ISButtonDialog *ButtonDialog;

	bool PasswordExist;
	unsigned int UserID;
	QString UserFIO;
	QString UserLogin;
};
//-----------------------------------------------------------------------------
#endif
