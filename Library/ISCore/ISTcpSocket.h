#pragma once
#ifndef _ISTCPSOCKET_H_INCLUDED
#define _ISTCPSOCKET_H_INCLUDED
//-----------------------------------------------------------------------------
#include "iscore_global.h"
//-----------------------------------------------------------------------------
class ISTcpSocket : public QTcpSocket
{
	Q_OBJECT

public:
	ISTcpSocket(qintptr SocketDescriptor, QObject *parent = 0);
	virtual ~ISTcpSocket();

private:
	void ReadyRead();

private:
	QByteArray Buffer;
};
//-----------------------------------------------------------------------------
#endif
