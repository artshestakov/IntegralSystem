#include "StdAfx.h"
#include "ISNotificationService.h"
#include "ISLocalization.h"
#include "ISSettings.h"
#include "ISConstants.h"
//-----------------------------------------------------------------------------
ISNotificationService::ISNotificationService()
{

}
//-----------------------------------------------------------------------------
ISNotificationService::~ISNotificationService()
{

}
//-----------------------------------------------------------------------------
void ISNotificationService::ShowNotification(ISNamespace::NotificationFormType NotificationType, const QString &MetaObjectName, const QString &ObjectName)
{
	if (!SETTING_BOOL(CONST_UID_SETTING_GENERAL_SHOWNOTIFICATIONFORM))
	{
		return;
	}

	ISPopupMessage *PopupMessage = new ISPopupMessage(nullptr);
	PopupMessage->SetMessage(ObjectName);

	switch (NotificationType)
	{
	case ISNamespace::NFT_Create:
		PopupMessage->SetTitle(LANG("NotificationForm.Title.Created") + " - " + MetaObjectName.toLower() + ":");
		break;

	case ISNamespace::NFT_CreateCopy:
		PopupMessage->SetTitle(LANG("NotificationForm.Title.CreatedCopy") + " - " + MetaObjectName.toLower() + ":");
		break;

	case ISNamespace::NFT_Edit:
		PopupMessage->SetTitle(LANG("NotificationForm.Title.Edited") + " - " + MetaObjectName.toLower() + ":");
		break;

	case ISNamespace::NFT_Delete:
		PopupMessage->SetTitle(LANG("NotificationForm.Title.Deleted"));
		break;

	case ISNamespace::NFT_Recovery:
		PopupMessage->SetTitle(LANG("NotificationForm.Title.Recovery"));
		break;
	}

	PopupMessage->show();
}
//-----------------------------------------------------------------------------
void ISNotificationService::ShowNotification(const QString &Title, const QString &Message)
{
	ISPopupMessage *PopupMessage = new ISPopupMessage(nullptr);
	PopupMessage->SetTitle(Title);
	PopupMessage->SetMessage(Message);
	PopupMessage->show();
}
//-----------------------------------------------------------------------------
void ISNotificationService::ShowNotification(const QString &Mesage)
{
	ISPopupMessage *PopupMessage = new ISPopupMessage(nullptr);
	PopupMessage->SetTitle(Mesage);
	PopupMessage->show();
}
//-----------------------------------------------------------------------------
