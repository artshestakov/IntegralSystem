#pragma once
#ifndef _ISTCPCONNECTOR_H_INCLUDED
#define _ISTCPCONNECTOR_H_INCLUDED
//-----------------------------------------------------------------------------
#include "iscore_global.h"
//-----------------------------------------------------------------------------
class ISCORE_EXPORT ISTcpConnector : public QObject
{
	Q_OBJECT

public:
	static ISTcpConnector& Instance();
	
	QString GetErrorString() const;
	QTcpSocket* GetSocket();
	bool IsConnected() const;
	bool Reconnect(const QString &host, quint16 port);
	bool Connect(const QString &host, quint16 port);
	void Disconnect();
	void Error(QTcpSocket::SocketError socket_error);

private:
	ISTcpConnector();
	~ISTcpConnector();
	ISTcpConnector(ISTcpConnector const &) {};
	ISTcpConnector& operator=(ISTcpConnector const&) { return *this; };

private:
	QString ErrorString;
	QTcpSocket *TcpSocket;
};
//-----------------------------------------------------------------------------
#endif
