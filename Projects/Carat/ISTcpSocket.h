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
	void Error(QAbstractSocket::SocketError socket_error); //Событие ошибки сокета
	ISNamespace::ApiMessageType GetMessageType(const QString &TypeName) const; //Получить тип сообщения по его имени
	void ClearBuffer(); //Очистка буфера

private:
	QByteArray Buffer;
	int MessageSize;
	int ChunkCount;
	QTimer *Timer;
};
//-----------------------------------------------------------------------------
#endif
