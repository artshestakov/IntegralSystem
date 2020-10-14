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
	void ClearBuffer(); //Очистка буфера
	void Send(const QVariantMap &Data); //Отправка данных
	void SendError(const QString &error_string); //Отправка ошибки
	ISNamespace::ApiMessageType GetMessageType(const QString &TypeName) const; //Получить тип сообщения по его имени

private:
	QByteArray Buffer;
};
//-----------------------------------------------------------------------------
#endif
