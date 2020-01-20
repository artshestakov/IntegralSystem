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
	void SendNotification(const QString &NotificationUID, const QString &NotificationName, int UserTo, const QVariant &Payload, const QString &String, bool Save); //������� ����������� ������������
	QVectorInt GetUsers() const; //�������� ������ ������������� ��� �������� �����������

private:
	QTimer *Timer;
};
//-----------------------------------------------------------------------------
