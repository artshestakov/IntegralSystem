#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISInterfaceDialogForm.h"
#include "ISPasswordEdit.h"
#include "ISCountingTime.h"
//-----------------------------------------------------------------------------
class ISLockForm : public ISInterfaceDialogForm
{
	Q_OBJECT

signals:
	void Unlocked();

public:
	ISLockForm(QWidget *parent = 0);
	virtual ~ISLockForm();

	void closeEvent(QCloseEvent *e);

protected:
	void AfterShowEvent() override;
	void Unlock();
	void EnterClicked() override;
	void NewNotify();

private:
	bool IsClose;
	ISPasswordEdit *EditPassword;
	QLabel *LabelNotify;

	int NewNotifyCount;
};
//-----------------------------------------------------------------------------
