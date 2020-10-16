#pragma once
#ifndef _ISTCPSOCKET_H_INCLUDED
#define _ISTCPSOCKET_H_INCLUDED
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISNamespace.h"
//-----------------------------------------------------------------------------
class ISTcpSocket : public QTcpSocket
{
	Q_OBJECT

public:
	ISTcpSocket(qintptr SocketDescriptor, QObject *parent = 0);
	virtual ~ISTcpSocket();

private:
	void ReadyRead(); //Событие входящих данных
	void SendErrorQuery(const QString &ErrorString); //Отправка ошибки клиенту
	ISNamespace::ApiMessageType GetMessageType(const QString &TypeName) const; //Получить тип сообщения по его имени

private:
	QByteArray Buffer;
};
//-----------------------------------------------------------------------------
#endif
