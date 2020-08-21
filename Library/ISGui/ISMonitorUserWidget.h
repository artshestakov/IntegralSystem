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
	ISMonitorUserWidget(bool is_online, int user_id, const QString &user_name, QWidget *parent = 0);
	virtual ~ISMonitorUserWidget();

protected:
	void enterEvent(QEvent *e);
	void leaveEvent(QEvent *e);
	void mouseDoubleClickEvent(QMouseEvent *e);
};
//-----------------------------------------------------------------------------
