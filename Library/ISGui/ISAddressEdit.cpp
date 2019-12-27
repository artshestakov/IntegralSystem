#include "StdAfx.h"
#include "ISAddressEdit.h"
#include "ISDefines.h"
#include "ISPushButton.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
#include "ISSystem.h"
#include "ISAddressForm.h"
#include "ISQuery.h"
//-----------------------------------------------------------------------------
ISAddressEdit::ISAddressEdit(QWidget *parent) : ISLineEdit(parent)
{
	ISPushButton *ButtonList = new ISPushButton(this);
	ButtonList->setText(LOCALIZATION("Address.ListFIAS"));
	ButtonList->setToolTip(LOCALIZATION("Address.ListFIAS"));
	ButtonList->setIcon(BUFFER_ICONS("Address"));
	connect(ButtonList, &ISPushButton::clicked, this, &ISAddressEdit::ListFIAS);
	AddWidgetToRight(ButtonList);
}
//-----------------------------------------------------------------------------
ISAddressEdit::~ISAddressEdit()
{

}
//-----------------------------------------------------------------------------
void ISAddressEdit::ListFIAS()
{
	ISSystem::SetWaitGlobalCursor(true);
	ISAddressForm AddressForm(GetLineEdit()->text());
	ISSystem::SetWaitGlobalCursor(false);
	if (AddressForm.Exec())
	{
		SetValue(AddressForm.GetString());
	}
}
//-----------------------------------------------------------------------------
