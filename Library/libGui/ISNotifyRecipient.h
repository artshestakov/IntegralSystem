#pragma once
//-----------------------------------------------------------------------------
#include "libgui_global.h"
#include "ISUuid.h"
//-----------------------------------------------------------------------------
class LIBGUI_EXPORT ISNotifyRecipient : public QObject
{
	Q_OBJECT

signals:

	//������������ ������� ��� �����������
	void ChatMessage(const QVariantMap &VariantMap); //������ � ����� ��������� � ����
	void TermianteUser(const QVariantMap &VariantMap); //������ � �������������� ���������� �� �������
	void UpdateAviable(const QVariantMap &VariantMap); //������ � ������� ����������
	void IncomingCall(const QVariantMap &VariantMap); //������ � �������� ������
	void CalendarEvent(const QVariantMap &VariantMap); //������ � ������� ���������
	void GetScreenshot(const QVariantMap &VariantMap); //������ � ��������� �������� �����
	void ScreenshotCreated(const QVariantMap &VariantMap); //������ � ���������� ��������� �������� ����� ������������
	void AlreadyConnected(const QVariantMap &VariantMap); //������ � ���, ��� ���-�� �������� �������������� ��� ��� �� �������

	//������� �������
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
	void Notification(const ISUuid &NotificationName, QSqlDriver::NotificationSource Source, const QVariant &Payload); //������� �����������

private:
	ISNotifyRecipient();
	QSqlDriver *SqlDriver;
};
//-----------------------------------------------------------------------------
