#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
//-----------------------------------------------------------------------------
class ISMonitorUserWidget : public QFrame
{
	Q_OBJECT

signals:
	void ShowUserCard();
	void ShowProtocol();

public:
	ISMonitorUserWidget(const QVariantMap &VariantMap, QWidget *parent);
	virtual ~ISMonitorUserWidget();

protected:
	void enterEvent(QEvent *e);
	void leaveEvent(QEvent *e);
	void mouseDoubleClickEvent(QMouseEvent *e);

private:
	void ServiceClicked();

private:
	bool IsOnline;
	unsigned int UserID;
	QString UserFIO;
	QString UserGroupName;
	QString IPAddress;
	int Port;
	QString DateTimeConnected;
	QString DateTimeLastQuery;
};
//-----------------------------------------------------------------------------
