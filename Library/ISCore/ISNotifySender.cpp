#include "StdAfx.h"
#include "ISNotifySender.h"
#include "ISDefines.h"
#include "ISAssert.h"
#include "ISQuery.h"
#include "ISDatabase.h"
#include "ISMetaUser.h"
#include "ISBuffer.h"
#include "ISQueryPool.h"
#include "ISSystem.h"
//-----------------------------------------------------------------------------
static QString QS_NOTIFICATIONS = PREPARE_QUERY("SELECT ntfn_uid, ntfn_name, ntfn_message, ntfn_soundfilename, ntfn_signalname, ntfn_showpopup "
												"FROM _notification "
												"WHERE NOT ntfn_isdeleted "
												"ORDER BY ntfn_id");
//-----------------------------------------------------------------------------
static QString QS_NOTIFICATION_NEW = PREPARE_QUERY("SELECT COUNT(*) "
													 "FROM _notificationuser "
													 "WHERE ntfu_userto = currentuserid()");
//-----------------------------------------------------------------------------
static QString QI_NOTIFICATION_USER = PREPARE_QUERY("INSERT INTO _notificationuser(ntfu_notificationid, ntfu_userto, ntfu_payload, ntfu_string, ntfu_save) "
													"VALUES((SELECT ntfn_id FROM _notification WHERE ntfn_uid = :NotificationUID), :UserTo, :Payload, :String, :Save)");
//-----------------------------------------------------------------------------
static QString QI_NOTIFICATION_USERS = PREPARE_QUERY("INSERT INTO _notificationuser(ntfu_notificationid, ntfu_userto, ntfu_payload, ntfu_string, ntfu_save) "
													 "SELECT (SELECT ntfn_id FROM _notification WHERE ntfn_uid = :NotificationUID), usrs_id, :Payload, :String, :Save "
													 "FROM _users "
													 "WHERE NOT usrs_isdeleted "
													 "AND usrs_id != currentuserid()");
//-----------------------------------------------------------------------------
ISNotifySender::ISNotifySender() : QObject()
{
	SqlDriver = ISDatabase::GetInstance().GetDefaultDB().driver();
	NewNotify = 0;
}
//-----------------------------------------------------------------------------
ISNotifySender::~ISNotifySender()
{

}
//-----------------------------------------------------------------------------
ISNotifySender& ISNotifySender::GetInstance()
{
	static ISNotifySender Notify;
	return Notify;
}
//-----------------------------------------------------------------------------
void ISNotifySender::Initialize()
{
	ISQuery qSelect(QS_NOTIFICATIONS);
	if (qSelect.Execute())
	{
		while (qSelect.Next())
		{
			ISUuid UID = qSelect.ReadColumn("ntfn_uid");
			QString Name = qSelect.ReadColumn("ntfn_name").toString();
			QString Message = qSelect.ReadColumn("ntfn_message").toString();
			QString SoundFileName = qSelect.ReadColumn("ntfn_soundfilename").toString();
			QString SignalName = qSelect.ReadColumn("ntfn_signalname").toString();
			bool ShowPopup = qSelect.ReadColumn("ntfn_showpopup").toBool();

			ISMetaNotify *MetaNotify = new ISMetaNotify(this);
			MetaNotify->SetUID(UID);
			MetaNotify->SetName(Name);
			MetaNotify->SetMessage(Message);
			MetaNotify->SetSoundFileName(SoundFileName);
			MetaNotify->SetSignalName(SignalName);
			MetaNotify->SetShowPopup(ShowPopup);
			Notifications.insert(UID, MetaNotify);
		}
	}

	ISQuery qSelectNewNotify(QS_NOTIFICATION_NEW);
	if (qSelectNewNotify.ExecuteFirst())
	{
		NewNotify = qSelectNewNotify.ReadColumn("count").toInt();
	}

	IS_ASSERT(SqlDriver->subscribeToNotification(CURRENT_USER_LOGIN), SqlDriver->lastError().text()); //Подключение к нотификации
}
//-----------------------------------------------------------------------------
int ISNotifySender::GetNewNotify() const
{
	return NewNotify;
}
//-----------------------------------------------------------------------------
ISMetaNotify* ISNotifySender::GetMetaNotify(const ISUuid &NotificationUID)
{
	ISMetaNotify *MetaNotify = Notifications.value(NotificationUID);
	IS_ASSERT(MetaNotify, QString("Not found notification from uid: %1").arg(NotificationUID));
	return MetaNotify;
}
//-----------------------------------------------------------------------------
void ISNotifySender::SendToAll(const ISUuid &NotificationUID, const QVariant &Payload, const QString &String, bool Save)
{
	SendNotification(NotificationUID, 0, Payload, String, Save);
}
//-----------------------------------------------------------------------------
void ISNotifySender::SendToUser(const ISUuid &NotificationUID, int UserID, const QVariant &Payload, const QString &String, bool Save)
{
	SendNotification(NotificationUID, UserID, Payload, String, Save);
}
//-----------------------------------------------------------------------------
void ISNotifySender::SendNotification(const ISUuid &NotificationUID, int UserID, const QVariant &Payload, const QString &String, bool Save)
{
	QVariantMap VariantMap;
	VariantMap.insert(":NotificationUID", NotificationUID);
	VariantMap.insert(":Payload", Payload);
	VariantMap.insert(":String", String);
	VariantMap.insert(":Save", Save);
	
	if (UserID) //Отправка конкретному пользователю
	{
		VariantMap.insert(":UserTo", UserID);
		ISQueryPool::GetInstance().AddQuery(QI_NOTIFICATION_USER, VariantMap);
	}
	else //Отправка всем пользователям кроме текущего
	{
		ISQueryPool::GetInstance().AddQuery(QI_NOTIFICATION_USERS, VariantMap);
	}
}
//-----------------------------------------------------------------------------
