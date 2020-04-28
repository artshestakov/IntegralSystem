#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISNamespace.h"

//-----------------------------------------------------------------------------
class ISNotificationService : public QObject
{
	Q_OBJECT

public:
	ISNotificationService();
	virtual ~ISNotificationService();

	static void ShowNotification(ISNamespace::NotificationFormType NotificationType, const QString &MetaObjectName = QString(), const QString &ObjectName = QString());
	static void ShowNotification(const QString &Title, const QString &Message);
	static void ShowNotification(const QString &Mesage);
};
//-----------------------------------------------------------------------------