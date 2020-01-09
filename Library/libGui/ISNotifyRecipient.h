#pragma once
//-----------------------------------------------------------------------------
#include "libgui_global.h"
#include "ISUuid.h"
//-----------------------------------------------------------------------------
class LIBGUI_EXPORT ISNotifyRecipient : public QObject
{
	Q_OBJECT

signals:

	//Персональные сигналы для нотификаций
	void ChatMessage(const QVariantMap &VariantMap); //Сигнал о новом сообщении в чате
	void TermianteUser(const QVariantMap &VariantMap); //Сигнал о принудительном отключении от системы
	void UpdateAviable(const QVariantMap &VariantMap); //Сигнал о наличии обновления
	void IncomingCall(const QVariantMap &VariantMap); //Сигнал о входящем звонке
	void CalendarEvent(const QVariantMap &VariantMap); //Сигнал о событии календаря
	void GetScreenshot(const QVariantMap &VariantMap); //Сигнал о получении рабочего стола
	void ScreenshotCreated(const QVariantMap &VariantMap); //Сигнал о готовности скриншота рабочего стола пользователя
	void AlreadyConnected(const QVariantMap &VariantMap); //Сигнал о том, что кто-то пытается авторизоваться под тем же логином

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
