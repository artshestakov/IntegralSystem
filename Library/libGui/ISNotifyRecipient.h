#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISUuid.h"
//-----------------------------------------------------------------------------
class ISNotifyRecipient : public QObject
{
	Q_OBJECT

signals:

	//Персональные сигналы для нотификаций
	void ChatMessage(const QVariantMap &VariantMap); //Сигнал о новом сообщении в чате
	void IncomingCall(const QVariantMap &VariantMap); //Сигнал о входящем звонке
	void CalendarEvent(const QVariantMap &VariantMap); //Сигнал о событии календаря

	//Базовые сигналы
	void Notify(const QVariantMap &VariantMap);
	void Notify();

public:
	ISNotifyRecipient(const ISNotifyRecipient &) = delete;
	ISNotifyRecipient(ISNotifyRecipient &&) = delete;
	ISNotifyRecipient &operator=(const ISNotifyRecipient &) = delete;
	ISNotifyRecipient &operator=(ISNotifyRecipient &&) = delete;
	~ISNotifyRecipient();

	static ISNotifyRecipient& GetInstance();

protected:
	void Notification(const ISUuid &NotificationName, QSqlDriver::NotificationSource Source, const QVariant &Payload); //Событие нотификации

private:
	ISNotifyRecipient();
	QSqlDriver *SqlDriver;
};
//-----------------------------------------------------------------------------
