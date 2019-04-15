#include "StdAfx.h"
#include "ISUrlEdit.h"
#include "EXDefines.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
#include "ISSystem.h"
#include "ISMessageBox.h"
#include "ISPopupMessage.h"
//-----------------------------------------------------------------------------
ISUrlEdit::ISUrlEdit(QWidget *parent) : ISLineEdit(parent)
{
	SetValidator(new QRegExpValidator(QRegExp(REG_EXP_URL), this));
	connect(this, &ISUrlEdit::DataChanged, this, &ISUrlEdit::UrlChanged);

	ButtonUrl = new ISServiceButton(this);
	ButtonUrl->setToolTip(LOCALIZATION("OpenUrlLink"));
	ButtonUrl->setIcon(BUFFER_ICONS("Url"));
	connect(ButtonUrl, &ISServiceButton::clicked, this, &ISUrlEdit::OpenUrl);
	AddWidgetToRight(ButtonUrl);
}
//-----------------------------------------------------------------------------
ISUrlEdit::~ISUrlEdit()
{

}
//-----------------------------------------------------------------------------
void ISUrlEdit::OpenUrl()
{
	QString UrlString = GetValue().toString();
	if (UrlString.length())
	{
		if (ISSystem::OpenUrl(UrlString))
		{
			ISPopupMessage *PopupMessage = new ISPopupMessage(nullptr);
			PopupMessage->SetTitle(LOCALIZATION("OpeningUrl") + "...");
			PopupMessage->SetMessage(UrlString);
			PopupMessage->show();
		}
		else
		{
			ISMessageBox::ShowWarning(nullptr, LOCALIZATION("Message.Warning.OpenUrl.Failed").arg(UrlString));
		}
	}
	else
	{
		ISMessageBox::ShowWarning(this, LOCALIZATION("Message.Warning.OpenUrl.Empty"));
		BlinkRed();
	}
}
//-----------------------------------------------------------------------------
void ISUrlEdit::UrlChanged()
{
	if (GetValue().toString().length())
	{
		ButtonUrl->setEnabled(true);
	}
	else
	{
		ButtonUrl->setEnabled(false);
	}
}
//-----------------------------------------------------------------------------
