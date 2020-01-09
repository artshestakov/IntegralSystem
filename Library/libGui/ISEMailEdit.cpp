#include "StdAfx.h"
#include "ISEMailEdit.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
#include "ISServiceButton.h"
#include "ISSystem.h"
#include "ISMessageBox.h"
//-----------------------------------------------------------------------------
ISEMailEdit::ISEMailEdit(QWidget *parent) : ISLineEdit(parent)
{
	setMaximumWidth(280);

	SetIcon(BUFFER_ICONS("EMail.FieldEdit.Acceptable"));
	SetPlaceholderText(LOCALIZATION("Field.EMail.PlaceholderText"));

	ISServiceButton *ButtonSend = new ISServiceButton(this);
	ButtonSend->setToolTip(LOCALIZATION("Send"));
	ButtonSend->setIcon(BUFFER_ICONS("EMailSend"));
	connect(ButtonSend, &ISServiceButton::clicked, this, &ISEMailEdit::Send);
	AddWidgetToRight(ButtonSend);
}
//-----------------------------------------------------------------------------
ISEMailEdit::~ISEMailEdit()
{

}
//-----------------------------------------------------------------------------
void ISEMailEdit::Send()
{
	QString EMail = GetValue().toString();
	if (EMail.length())
	{
		ISSystem::SetWaitGlobalCursor(true);
		QDesktopServices::openUrl(QUrl("mailto:" + EMail));
		ISSystem::SetWaitGlobalCursor(false);
	}
	else
	{
		ISMessageBox::ShowWarning(this, LOCALIZATION("Message.Warning.EMailIsEmpty"));
		BlinkRed();
	}
}
//-----------------------------------------------------------------------------
