#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISInterfaceForm.h"
#include "ISButtons.h"
#include "ISListWidget.h"

//-----------------------------------------------------------------------------
class ISNotificationsForm : public ISInterfaceForm
{
	Q_OBJECT

signals:
	void CountChanged(int Count);

public:
	ISNotificationsForm(QWidget *parent = 0);
	virtual ~ISNotificationsForm();

	void showEvent(QShowEvent *ShowEvent);

protected:
	void paintEvent(QPaintEvent *e);

private:
	void ReloadNotify();
	void Clear();
	void DeleteNotifyUser(int ID);

private:
	ISListWidget *ListWidget;
	ISPushButton *ButtonClear;
	QLabel *LabelEmpty;
};
//-----------------------------------------------------------------------------
