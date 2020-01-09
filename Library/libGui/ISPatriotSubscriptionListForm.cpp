#include "StdAfx.h"
#include "ISPatriotSubscriptionListForm.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
#include "ISInputDialog.h"
#include "ISPatriot.h"
#include "ISObjects.h"
//-----------------------------------------------------------------------------
ISPatriotSubscriptionListForm::ISPatriotSubscriptionListForm(QWidget *parent) : ISListBaseForm("Subscriptions", parent)
{
	QAction *ActionRegister = new QAction(this);
	ActionRegister->setText(LOCALIZATION("Patriot.Register"));
	ActionRegister->setToolTip(LOCALIZATION("Patriot.Register"));
	ActionRegister->setIcon(ISObjects::GetInstance().GetInterface()->GetIcon("Register"));
	connect(ActionRegister, &QAction::triggered, this, &ISPatriotSubscriptionListForm::Register);
	AddAction(ActionRegister, false);
}
//-----------------------------------------------------------------------------
ISPatriotSubscriptionListForm::~ISPatriotSubscriptionListForm()
{

}
//-----------------------------------------------------------------------------
void ISPatriotSubscriptionListForm::Register()
{
	QVariant Barcode = ISInputDialog::GetString(this, LOCALIZATION("Patriot.Register"), LOCALIZATION("Patriot.InputBarcode"));
	if (Barcode.isValid())
	{
		dynamic_cast<ISPatriot*>(ISObjects::GetInstance().GetInterface())->InputScannerData(Barcode.toString());
	}
}
//-----------------------------------------------------------------------------
