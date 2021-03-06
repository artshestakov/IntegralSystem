#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISInterfaceDialogForm.h"
#include "ISUserEdit.h"
#include "ISDateEdit.h"
//-----------------------------------------------------------------------------
class ISTransferOrganizationForm : public ISInterfaceDialogForm
{
	Q_OBJECT

public:
	ISTransferOrganizationForm(int OrganizationCount);
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
