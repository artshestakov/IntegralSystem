#pragma once
#ifndef _ISTCPSERVERCARAT_H_INCLUDED
#define _ISTCPSERVERCARAT_H_INCLUDED
//-----------------------------------------------------------------------------
#include "ISTcpServerBase.h"
//-----------------------------------------------------------------------------
class ISCORE_EXPORT ISTcpServerCarat : public ISTcpServerBase
{
	Q_OBJECT

public:
	ISTcpServerCarat(QObject *parent = 0);
	virtual ~ISTcpServerCarat();

protected:
	void incomingConnection(qintptr SocketDescriptor) override; //Событие входящего соединения

private:
	void SendError(QTcpSocket *TcpSocket, const QString &ErrorString); //Отправка ошибки
};
//-----------------------------------------------------------------------------

#endif
