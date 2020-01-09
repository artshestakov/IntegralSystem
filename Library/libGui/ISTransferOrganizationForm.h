#pragma once
//-----------------------------------------------------------------------------
#include "libgui_global.h"
#include "ISInterfaceDialogForm.h"
#include "ISUserEdit.h"
#include "ISDateEdit.h"
//-----------------------------------------------------------------------------
class LIBGUI_EXPORT ISTransferOrganizationForm : public ISInterfaceDialogForm
{
	Q_OBJECT

public:
	ISTransferOrganizationForm(int OrganizationCount, QWidget *parent = 0);
	virtual ~ISTransferOrganizationForm();

	int GetSelectedUserID() const;
	QString GetSelectedUserName() const;
	QDate GetSelectedDate() const;

protected:
	void Select();

private:
	ISUserEdit *UserEdit;
	ISDateEdit *DateEdit;

	int SelectedUserID;
};
//-----------------------------------------------------------------------------
