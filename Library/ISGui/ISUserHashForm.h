#pragma once
#ifndef _ISUSERHASHFORM_H_INCLUDED
#define _ISUSERHASHFORM_H_INCLUDED
//-----------------------------------------------------------------------------
#include "ISInterfaceDialogForm.h"
#include "ISFieldEdits.h"
//-----------------------------------------------------------------------------
class ISUserHashForm : public ISInterfaceDialogForm
{
	Q_OBJECT

public:
	ISUserHashForm(unsigned int user_id, const QString &user_fio);
	virtual ~ISUserHashForm();

protected:
	void AfterShowEvent() override;
	void EnterClicked() override;

private:
	void Apply();
	void SetIconAndTextHeader(const QIcon &Icon, const QString &Text);

private:
	ISLineEdit *EditLoginCurrent;
	ISLineEdit *EditPasswordCurrent;
	ISLineEdit *EditLogin;
	ISLineEdit *EditPassword;
	ISLineEdit *EditPasswordCheck;
	QLabel *LabelText;
	ISButtonDialog *ButtonDialog;

	unsigned int UserID;
	QString UserFIO;
	bool LoginExist;
};
//-----------------------------------------------------------------------------
#endif
