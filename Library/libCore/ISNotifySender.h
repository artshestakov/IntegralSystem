#pragma once
//-----------------------------------------------------------------------------
#include "libcore_global.h"
#include "ISMetaNotify.h"
#include "ISUuid.h"
//-----------------------------------------------------------------------------
class LIBCORE_EXPORT ISNotifySender : public QObject
{
	Q_OBJECT

public:
	ISNotifySender(const ISNotifySender &) = delete;
	ISNotifySender(ISNotifySender &&) = delete;
	ISNotifySender &operator=(const ISNotifySender &) = delete;
	ISNotifySender &operator=(ISNotifySender &&) = delete;
	~ISNotifySender();

	static ISNotifySender& GetInstance();

	void Initialize(); //Инициализация
	int GetNewNotify() const; //Получить количество новых уведомлений
	ISMetaNotify* GetMetaNotify(const ISUuid &NotificationUID); //Получить нотификацию по её идентификатору

	void SendToAll(const ISUuid &NotificationUID, const QVariant &Payload = QVariant(), const QString &String = QString(), bool Save = true); //Отправить всем кроме себя
	void SendToUser(const ISUuid &NotificationUID, int UserID, const QVariant &Payload = QVariant(), const QString &String = QString(), bool Save = true); //Отправить для пользователя

protected:
	void SendNotification(const ISUuid &NotificationUID, int UserID, const QVariant &Payload, const QString &String, bool Save); //Отправить нотификацию

private:
	ISNotifySender();
	QMap<QString, ISMetaNotify*> Notifications;
	QSqlDriver *SqlDriver;
	int NewNotify;
};
//-----------------------------------------------------------------------------
