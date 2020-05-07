#pragma once
#ifndef _ISTCPSERVER_H_INCLUDED
#define _ISTCPSERVER_H_INCLUDED
//-----------------------------------------------------------------------------
#include "iscore_global.h"
//-----------------------------------------------------------------------------
class ISCORE_EXPORT ISTcpServer : public QTcpServer
{
	Q_OBJECT

public:
	ISTcpServer(QObject *parent = 0);
	virtual ~ISTcpServer();

	QString GetErrorString() const;
	bool Run(quint16 Port);

protected:
	void incomingConnection(qintptr SocketDescriptor) override; //Событие входящего соединения

private:
	void AcceptError(QAbstractSocket::SocketError socket_error);
	void SendError(QTcpSocket *TcpSocket, QString ErrorString);

private:
	QString ErrorString;
};
//-----------------------------------------------------------------------------
#endif
