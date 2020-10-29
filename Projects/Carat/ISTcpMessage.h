#pragma once
#ifndef _ISTCPMESSAGE_H_INCLUDED
#define _ISTCPMESSAGE_H_INCLUDED
//-----------------------------------------------------------------------------
#include "ISTcpSocket.h"
//-----------------------------------------------------------------------------
class ISTcpMessage
{
public:
	ISTcpMessage(ISTcpSocket *tcp_socket);
	~ISTcpMessage();

	QString GetErrorString() const; //Получить текстовое описание ошибки
	void SetErrorString(const QString &error_string); //Изменить текстовое описание ошибки
	bool IsValid() const; //Получить валидность сообщения

	ISNamespace::ApiMessageType Type; //Тип сообщения
	QString TypeName; //Имя типа сообщения
	QVariantMap Parameters; //Параметры
	ISTcpSocket *TcpSocket; //Указатель на клиента

	long long ParseMSec; //Время парсинга
	unsigned int Size; //Размер сообщения
	unsigned int ChunkCount; //Количество чанков
	qint64 MessageID; //Идентификатор сообщения

private:
	QString ErrorString;
	bool Valid; //Флаг валидности сообщения
};
//-----------------------------------------------------------------------------
#endif
