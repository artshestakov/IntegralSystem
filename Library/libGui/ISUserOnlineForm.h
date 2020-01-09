#pragma once
//-----------------------------------------------------------------------------
#include "libgui_global.h"
#include "ISInterfaceForm.h"
#include "ISDateEdit.h"
#include "ISListWidget.h"
//-----------------------------------------------------------------------------
class LIBGUI_EXPORT ISUserOnlineForm : public ISInterfaceForm
{
	Q_OBJECT

public:
	ISUserOnlineForm(int user_id, const QString &user_name, QWidget *parent = 0);
	virtual ~ISUserOnlineForm();

protected:
	void Update();
	void EscapeClicked() override;

private:
	int UserID;

	ISDateEdit *DateEdit;
	QLabel *LabelCountTime;
	QLabel *LabelNotRegistered;
	ISListWidget *ListWidget;
};
//-----------------------------------------------------------------------------
