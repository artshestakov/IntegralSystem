#pragma once
//-----------------------------------------------------------------------------
#include "libgui_global.h"
#include "ISUuid.h"
//-----------------------------------------------------------------------------
class LIBGUI_EXPORT ISNotifyWidgetItem : public QWidget
{
	Q_OBJECT

signals:
	void Delete(int ID);

public:
	ISNotifyWidgetItem(QWidget *parent, int notification_user_id, const ISUuid &NotificationUID, const QDateTime &DateTime, const QString &PayloadString);
	virtual ~ISNotifyWidgetItem();

protected:
	void DeleteClicked();

private:
	int ID;
};
//-----------------------------------------------------------------------------
