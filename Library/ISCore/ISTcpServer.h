#pragma once
#ifndef _ISTCPSERVERCARAT_H_INCLUDED
#define _ISTCPSERVERCARAT_H_INCLUDED
//-----------------------------------------------------------------------------
#include "iscore_global.h"
#include "ISTcpSocket.h"
//-----------------------------------------------------------------------------
class ISCORE_EXPORT ISTcpServer : public QTcpServer
{
	Q_OBJECT

public:
	ISTcpServer(QObject *parent = 0);
	virtual ~ISTcpServer();

	QString GetErrorString() const;
	bool Run(quint16 Port);

private:
	void incomingConnection(qintptr SocketDescriptor) override; //Событие входящего соединения
	void ClientDisconnected(); //Событие отключения клиента
	void ClientError(QAbstractSocket::SocketError socket_error); //Ошибка клиента
	void AcceptError(QTcpSocket::SocketError socket_error); //Ошибка принятия подключения

	

private:
	QString ErrorString;
	QString DBHost;
	int DBPort;
	QString DBName;
	std::vector<ISTcpSocket *> Clients;
};
//-----------------------------------------------------------------------------
#endif
