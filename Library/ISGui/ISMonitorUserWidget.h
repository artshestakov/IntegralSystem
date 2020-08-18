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
	void ShowDetails();

public:
	ISMonitorUserWidget(bool is_online, int user_id, const QString &user_name, QWidget *parent = 0);
	virtual ~ISMonitorUserWidget();

	int GetUserID() const;
	QString GetUserName() const;

protected:
	void enterEvent(QEvent *e);
	void leaveEvent(QEvent *e);
	void mouseDoubleClickEvent(QMouseEvent *e);

private:
	int UserID;
	QString UserName;
};
//-----------------------------------------------------------------------------
