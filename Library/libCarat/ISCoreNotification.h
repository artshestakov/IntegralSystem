#pragma once
//-----------------------------------------------------------------------------
#include "ISCaratCoreApplication.h"
#include "ISTypes.h"
//-----------------------------------------------------------------------------
class ISCoreNotification : public ISCaratCoreApplication
{
	Q_OBJECT

public:
	ISCoreNotification(int &argc, char **argv);
	virtual ~ISCoreNotification();

	bool Invoke() override;

protected:
	void Timeout();
	void SendNotification(const QString &NotificationUID, const QString &NotificationName, int UserTo, const QVariant &Payload, const QString &String, bool Save); //Отпрвка нотификации пользователю
	QVectorInt GetUsers() const; //Получить список пользователей для отправки нотификации

private:
	QTimer *Timer;
};
//-----------------------------------------------------------------------------
