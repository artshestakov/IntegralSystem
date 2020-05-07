#pragma once
#ifndef _ISTCPSERVERBASE_H_INCLUDED
#define _ISTCPSERVERBASE_H_INCLUDED
//-----------------------------------------------------------------------------
#include "iscore_global.h"
//-----------------------------------------------------------------------------
class ISCORE_EXPORT ISTcpServerBase : public QTcpServer
{
	Q_OBJECT

public:
	ISTcpServerBase(QObject *parent = 0);
	virtual ~ISTcpServerBase();

	QString GetErrorString() const;
	bool Run(qint16 Port);

protected:
	void Send(QTcpSocket *TcpSocket, const QVariantMap &Data); //Отправка данных

private:
	void AcceptError(QAbstractSocket::SocketError);

private:
	QString ErrorString;
};
//-----------------------------------------------------------------------------
#endif
