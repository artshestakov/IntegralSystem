#pragma once
//-----------------------------------------------------------------------------
#include "libgui_global.h"
#include "ISInterfaceDialogForm.h"
#include "ISLineEdit.h"
#include "ISDateEdit.h"
#include "ISTextEdit.h"
#include "ISDivisionCodeEdit.h"
//-----------------------------------------------------------------------------
class LIBGUI_EXPORT ISPassportForm : public ISInterfaceDialogForm
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
