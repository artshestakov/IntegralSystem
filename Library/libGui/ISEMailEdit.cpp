#include "ISEMailEdit.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
#include "ISButtons.h"
#include "ISGui.h"
#include "ISMessageBox.h"
//-----------------------------------------------------------------------------
ISEMailEdit::ISEMailEdit(QWidget *parent) : ISLineEdit(parent)
{
	setMaximumWidth(280);

	SetIcon(BUFFER_ICONS("EMail.FieldEdit.Acceptable"));
	SetPlaceholderText(LANG("Field.EMail.PlaceholderText"));

	ISServiceButton *ButtonSend = new ISServiceButton(this);
	ButtonSend->setToolTip(LANG("Send"));
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
		ISGui::SetWaitGlobalCursor(true);
		QDesktopServices::openUrl(QUrl("mailto:" + EMail));
		ISGui::SetWaitGlobalCursor(false);
	}
	else
	{
		ISMessageBox::ShowWarning(this, LANG("Message.Warning.EMailIsEmpty"));
		BlinkRed();
	}
}
//-----------------------------------------------------------------------------
