#include "StdAfx.h"
#include "ISCoreNotification.h"
#include "ISDefines.h"
#include "ISDatabase.h"
#include "ISQuery.h"
#include "ISSystem.h"
#include "ISDebug.h"
#include "ISLocalization.h"
//-----------------------------------------------------------------------------
static QString QS_USERS = PREPARE_QUERY("SELECT usrs_id "
										"FROM _users "
										"WHERE NOT usrs_isdeleted");
//-----------------------------------------------------------------------------
static QString PG_NOTIFY = PREPARE_QUERY("SELECT pg_notify((SELECT usrs_login FROM _users WHERE usrs_id = :UserTo), :Payload), userfullname(:UserTo) ");
//-----------------------------------------------------------------------------
static QString QS_NOTIFICATION_USER = PREPARE_QUERY("SELECT ntfu_id, ntfn_uid, ntfn_name, ntfu_userto, ntfu_payload, ntfu_string, ntfu_save "
													"FROM _notificationuser "
													"LEFT JOIN _notification ON ntfn_id = ntfu_notificationid "
													"WHERE NOT ntfu_sended "
													"ORDER BY ntfu_id");
//-----------------------------------------------------------------------------
static QString QU_NOTIFICATION_SENDED = PREPARE_QUERY("UPDATE _notificationuser SET "
													  "ntfu_sended = true "
													  "WHERE ntfu_id = :NotificationID");
//-----------------------------------------------------------------------------
static QString QD_NOTIFICATION = PREPARE_QUERY("DELETE FROM _notificationuser "
											   "WHERE ntfu_id = :NotificationID");
//-----------------------------------------------------------------------------
ISCoreNotification::ISCoreNotification(int &argc, char **argv) : ISCaratCoreApplication(argc, argv)
{
	
}
//-----------------------------------------------------------------------------
ISCoreNotification::~ISCoreNotification()
{
	
}
//-----------------------------------------------------------------------------
bool ISCoreNotification::Invoke()
{
	bool Result = ISCaratCoreApplication::Invoke();
	if (Result)
	{
		Timer = new QTimer(this);
		Timer->setInterval(100);
		connect(Timer, &QTimer::timeout, this, &ISCoreNotification::Timeout);
        Timer->start();

		Started();
	}

	return Result;
}
//-----------------------------------------------------------------------------
void ISCoreNotification::Timeout()
{
	Timer->stop();

	ISQuery qSelect(QS_NOTIFICATION_USER);
	if (qSelect.Execute())
	{
		while (qSelect.Next())
		{
			int NotificationUserID = qSelect.ReadColumn("ntfu_id").toInt();
			ISUuid NotificationUID = qSelect.ReadColumn("ntfn_uid"); //Идентификатор нотификации
			QString NotificationName = qSelect.ReadColumn("ntfn_name").toString(); //Наименование нотификации
			int UserTo = qSelect.ReadColumn("ntfu_userto").toInt(); //Кому отправлено
			QVariant Payload = qSelect.ReadColumn("ntfu_payload");
			QString String = qSelect.ReadColumn("ntfu_string").toString();
			bool Save = qSelect.ReadColumn("ntfu_save").toBool();

			if (UserTo) //Если нотификация отправлена конкретному пользователю
			{
				SendNotification(NotificationUID, NotificationName, UserTo, Payload, String, Save);
			}
			else //Нотфикация отправлена всем пользователям
			{
				QVectorInt Users = GetUsers();
				for (int UserID : Users) //Обход пользователей и отправка каждому нотификации
				{
					SendNotification(NotificationUID, NotificationName, UserTo, Payload, String, Save);
				}
			}

			if (Save) //Если уведомление нужно сохранить в базе - обновляем его статус "Отправлено"
			{
				ISQuery qSended(QU_NOTIFICATION_SENDED);
				qSended.BindValue(":NotificationID", NotificationUserID);
				qSended.Execute();
			}
			else //Сохранять уведомление в базе не нужно - удаляем его
			{
				ISQuery qDelete(QD_NOTIFICATION);
				qDelete.BindValue(":NotificationID", NotificationUserID);
				qDelete.Execute();
			}
		}
	}

	Timer->start();
}
//-----------------------------------------------------------------------------
void ISCoreNotification::SendNotification(const QString &NotificationUID, const QString &NotificationName, int UserTo, const QVariant &Payload, const QString &String, bool Save)
{
	//Сериализация данных
	QVariantMap VariantMap;
	VariantMap.insert("NotificationUID", NotificationUID); //Кому отправлена нотификация
	VariantMap.insert("Payload", Payload); //Информация по нотификакции
	VariantMap.insert("String", String); //Текстовое сообщение
	VariantMap.insert("Save", Save);

	ISQuery qNotify(PG_NOTIFY);
	qNotify.BindValue(":UserTo", UserTo);
	qNotify.BindValue(":Payload", ISSystem::VariantMapToJsonString(VariantMap));
	if (qNotify.ExecuteFirst()) //Если нотификация отправлена успешно
	{
		ISDebug::ShowString(LANG("Notification.Sended").arg(NotificationName).arg(qNotify.ReadColumn("userfullname").toString()));
	}
}
//-----------------------------------------------------------------------------
QVectorInt ISCoreNotification::GetUsers() const
{
	QVectorInt VectorInt;
	ISQuery qSelectUsers(QS_USERS);
	if (qSelectUsers.Execute())
	{
		while (qSelectUsers.Next())
		{
			VectorInt.append(qSelectUsers.ReadColumn("usrs_id").toInt());
		}
	}

	return VectorInt;
}
//-----------------------------------------------------------------------------
