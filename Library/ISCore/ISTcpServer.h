#pragma once
#ifndef _ISTCPSERVER_H_INCLUDED
#define _ISTCPSERVER_H_INCLUDED
//-----------------------------------------------------------------------------
#include "iscore_global.h"
//-----------------------------------------------------------------------------
class ISCORE_EXPORT ISTcpServer : public QObject
{
	Q_OBJECT

public:
	ISTcpServer(QObject *parent);
	virtual ~ISTcpServer();

	QString GetErrorString() const;
	bool Run(quint16 Port);

private:
	void NewConnection();
	void AcceptError(QAbstractSocket::SocketError socket_error);
	void Disconnect();

private:
	QString ErrorString;
	QTcpServer *TcpServer;
};
//-----------------------------------------------------------------------------
#endif
