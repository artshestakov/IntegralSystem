#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISInterfaceDialogForm.h"
//-----------------------------------------------------------------------------
class ISUserOnlineDetailsForm : public ISInterfaceDialogForm
{
	Q_OBJECT

public:
	ISUserOnlineDetailsForm(int user_id);
	virtual ~ISUserOnlineDetailsForm();

protected:
	void AddLabel(const QString &LabelName, const QString &LabelValue);

	QString GetState(const QString &State) const;

private:
	QFormLayout *FormLayout;
};
//-----------------------------------------------------------------------------
