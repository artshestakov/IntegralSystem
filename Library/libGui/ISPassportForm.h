#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISInterfaceDialogForm.h"
#include "ISFieldEdits.h"
//-----------------------------------------------------------------------------
class ISPassportForm : public ISInterfaceDialogForm
{
	Q_OBJECT

public:
	ISPassportForm(const QString &PassportString, QWidget *parent = 0);
	virtual ~ISPassportForm();

	QString GetPassportString();

protected:
	void AfterShowEvent() override;
	void EnterClicked() override;
	void Apply();
	void ParsePassportString(const QString &PassportString);

private:
	ISLineEdit *EditSeria;
	ISLineEdit *EditNumber;
	ISDateEdit *EditDateOfIssue;
	ISTextEdit *EditIssuedBy;
	ISDivisionCodeEdit *EditDivisionCode;
};
//-----------------------------------------------------------------------------
