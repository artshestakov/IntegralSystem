#pragma once
//-----------------------------------------------------------------------------
#include "libCarat_global.h"
#include "ISTypes.h"
//-----------------------------------------------------------------------------
class LIBCARAT_EXPORT ISAsteriskSocket : public QTcpSocket
{
	Q_OBJECT

signals:
	void Disconnected();

	void SuccessfulAuth(const ISStringMap &StringMap);
	void UserEvent(const ISStringMap &StringMap);
	void BeforeDialBegin(const ISStringMap &StringMap);
	void DialBegin(const ISStringMap &StringMap);
	void AfterDialBegin(const ISStringMap &StringMap);
	void DialEnd(const ISStringMap &StringMap);
	void BlindTransfer(const ISStringMap &StringMap);
	void Hangup(const ISStringMap &StringMap);
	void ExtensionStatus(const ISStringMap &StringMap);

public:
	ISAsteriskSocket(QObject *parent = 0);
	virtual ~ISAsteriskSocket();

	void Connect(); //������������ � ������� ���������
	void AddFilter(const QString &EventName); //�������� ���������� �������
	void Redirect(const ISStringMap &StringMap, const QString &Pattern); //��������������� ������

protected:
	void ConnectedHost(); //������� ����������� � ������� ���������
	void DisconnectedHost(); //������� ���������� �� ������� ���������
	void StateChangedHost(QAbstractSocket::SocketState NewState);
	void ErrorHost(QAbstractSocket::SocketError ErrorHost);
	
	void ReadyRead(); //�������� ��������� �� AMI
	QVector<ISStringMap> ParseReadyRead(const QString &String); //������� �������� ��������� �� AMI

	void Timeout();

private:
	QString Host;
	int Port;
	QString Login;
	QString Password;

	QTimer *Timer;
};
//-----------------------------------------------------------------------------
