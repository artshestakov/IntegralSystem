#pragma once
//-----------------------------------------------------------------------------
#include "isintegralsystem_global.h"
#include "ISNamespace.h"
//-----------------------------------------------------------------------------
class ISINTEGRALSYSTEM_EXPORT ISMonitorUserWidget : public QFrame
{
	Q_OBJECT

signals:
	void ShowUserCard();
	void ShowActivity();
	void ShowProtocol();
	void ShowDetails();
	void EndSession();
	void GetScreenshot();
	void SendNotify();

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
