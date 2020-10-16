#pragma once
#ifndef _ISTCPMESSAGE_H_INCLUDED
#define _ISTCPMESSAGE_H_INCLUDED
//-----------------------------------------------------------------------------
#include "ISTcpSocket.h"
//-----------------------------------------------------------------------------
class ISTcpMessage
{
public:
	ISTcpMessage(ISNamespace::ApiMessageType type, const QVariantMap &parameters, ISTcpSocket *tcp_socket);
	ISTcpMessage(ISTcpSocket *tcp_socket, const QString &error_string);
	~ISTcpMessage();

	QString GetErrorString() const; //Получить текстовое описание ошибки
	bool IsValid() const; //Получить валидность сообщения

	ISNamespace::ApiMessageType Type; //Тип сообщения
	QVariantMap Parameters; //Параметры
	ISTcpSocket *TcpSocket; //Указатель на клиента

private:
	QString ErrorString;
	bool Valid; //Флаг валидности сообщения
};
//-----------------------------------------------------------------------------
#endif
