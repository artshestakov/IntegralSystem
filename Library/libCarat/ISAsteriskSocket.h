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

	void SuccessfulAuth(const QStringMap &StringMap);
	void UserEvent(const QStringMap &StringMap);
	void BeforeDialBegin(const QStringMap &StringMap);
	void DialBegin(const QStringMap &StringMap);
	void AfterDialBegin(const QStringMap &StringMap);
	void DialEnd(const QStringMap &StringMap);
	void BlindTransfer(const QStringMap &StringMap);
	void Hangup(const QStringMap &StringMap);
	void ExtensionStatus(const QStringMap &StringMap);

public:
	ISAsteriskSocket(QObject *parent = 0);
	virtual ~ISAsteriskSocket();

	void Connect(); //Подключиться к серверу телефонии
	void AddFilter(const QString &EventName); //Добавить фильтрацию события
	void Redirect(const QStringMap &StringMap, const QString &Pattern); //Перенаправление вызова

protected:
	void ConnectedHost(); //Событие подключения к серверу телефонии
	void DisconnectedHost(); //Событие отключения от сервера телефонии
	void StateChangedHost(QAbstractSocket::SocketState NewState);
	void ErrorHost(QAbstractSocket::SocketError ErrorHost);
	
	void ReadyRead(); //Входящее сообщение от AMI
	QVector<QStringMap> ParseReadyRead(const QString &String); //Парсинг входящих сообщений от AMI

	void Timeout();

private:
	QString Host;
	int Port;
	QString Login;
	QString Password;

	QTimer *Timer;
};
//-----------------------------------------------------------------------------
