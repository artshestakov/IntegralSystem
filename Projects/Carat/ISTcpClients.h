#pragma once
#ifndef _ISTCPCLIENTS_H_INCLUDED
#define _ISTCPCLIENTS_H_INCLUDED
//-----------------------------------------------------------------------------
#include "ISStructs.h"
//-----------------------------------------------------------------------------
class ISTcpClients
{
public:
	static ISTcpClients& Instance();

	void Add(qintptr SocketDescriptor, unsigned int UserID, const QString &Address, unsigned short Port); //Добавить клиента
	void Remove(qintptr SocketDescriptor); //Удалить клиента
	ISVectorUInt GetClientsID(); //Получить список идентификаторов клиентов
	ISClientInfo GetClient(unsigned int UserID); //Получить информацию о клиенте по его идентификатору
	bool ExistUserID(unsigned int UserID); //Проверить наличие такого пользователя
	size_t GetCount(); //Получить количество клиентов

	void UpdateLastQuery(qintptr SocketDescriptor, ISNamespace::ApiMessageType MessageType, bool Result, unsigned long long MSec); //Одновить дату и результат последнего запроса

private:
	ISTcpClients();
	~ISTcpClients();
	ISTcpClients(ISTcpClients const &) {};
	ISTcpClients& operator=(ISTcpClients const&) { return *this; };

private:
	std::map<qintptr, ISClientInfo> Clients;
	ISCriticalSection CriticalSection;
};
//-----------------------------------------------------------------------------
#endif
