#pragma once
//-----------------------------------------------------------------------------
#include "libgui_global.h"
#include "ISInterfaceDialogForm.h"
#include "ISLineEdit.h"
#include "ISPasswordWidthWidget.h"
#include "ISButtonDialog.h"
//-----------------------------------------------------------------------------
class LIBGUI_EXPORT ISUserPasswordForm : public ISInterfaceDialogForm
{
	Q_OBJECT

public:
	ISUserPasswordForm(int user_id, QWidget *parent = 0);
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
