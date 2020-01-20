#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISInterfaceDialogForm.h"
//-----------------------------------------------------------------------------
class ISIncomingCallBaseForm : public ISInterfaceDialogForm
{
	Q_OBJECT

public:
	ISIncomingCallBaseForm(const QVariantMap &payload, QWidget *parent = 0);
	virtual ~ISIncomingCallBaseForm();

protected:
	void AfterShowEvent() override;
	void Timeout();
	void AddLabel(const QString &LabelName, const QString &LabelText);

	virtual void OpenCard();

private:
	QVariantMap Payload;
	QLabel *LabelTimeout;
	QGroupBox *GroupBox;

	int SettingTimeout;
	int CurrentTimeout;
};
//-----------------------------------------------------------------------------
