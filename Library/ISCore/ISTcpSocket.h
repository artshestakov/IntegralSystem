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
	void ReadyRead(); //Событие входящих данных
	void ClearBuffer(); //Очистка буфера
	void Send(const QVariantMap &Data); //Отправка данных
	void SendError(const QString &error_string); //Отправка ошибки

private:
	QByteArray Buffer;
	bool IsAuthorized;
};
//-----------------------------------------------------------------------------
#endif
