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

	void SetAuthorized(bool authorized);
	bool GetAuthorized() const;

	void SetUserID(int user_id);
	int GetUserID() const;

	void SetUserGroupID(int user_group_id);
	int GetUserGroupID() const;

	void SetUserIsSystem(bool user_is_system);
	bool GetUserIsSystem() const;

private:
	void ReadyRead(); //Событие входящих данных
	void Error(QAbstractSocket::SocketError socket_error); //Событие ошибки сокета
	ISNamespace::ApiMessageType GetMessageType(const QString &TypeName) const; //Получить тип сообщения по его имени
	void ClearBuffer(); //Очистка буфера
	void Timeout(); //Событие таймаута

private:
	QByteArray Buffer; //Буфер
	unsigned int MessageSize; //Размер сообщения
	unsigned int ChunkCount; //Количество частей сообщения
	QTimer *Timer;

	bool IsAuthorized; //Флаг авторизации клиента
	int UserID; //Идентификатор пользователя
	int UserGroupID; //Идентификатор группы пользователя
	bool UserIsSystem; //Флаг системного пользователя
};
//-----------------------------------------------------------------------------
#endif
