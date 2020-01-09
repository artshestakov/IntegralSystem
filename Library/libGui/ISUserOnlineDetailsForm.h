#pragma once
//-----------------------------------------------------------------------------
#include "libgui_global.h"
#include "ISInterfaceDialogForm.h"
//-----------------------------------------------------------------------------
class LIBGUI_EXPORT ISUserOnlineDetailsForm : public ISInterfaceDialogForm
{
	Q_OBJECT

public:
	ISUserOnlineDetailsForm(int user_id, QWidget *parent = 0);
	virtual ~ISUserOnlineDetailsForm();

protected:
	void AddLabel(const QString &LabelName, const QString &LabelValue);

	QString GetState(const QString &State) const;

private:
	QFormLayout *FormLayout;
};
//-----------------------------------------------------------------------------
