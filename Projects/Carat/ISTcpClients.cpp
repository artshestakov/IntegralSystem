#include "ISTcpClients.h"
#include "ISAlgorithm.h"
//-----------------------------------------------------------------------------
ISTcpClients::ISTcpClients()
{
	CRITICAL_SECTION_INIT(&CriticalSection);
}
//-----------------------------------------------------------------------------
ISTcpClients::~ISTcpClients()
{
	CRITICAL_SECTION_DESTROY(&CriticalSection);
}
//-----------------------------------------------------------------------------
ISTcpClients& ISTcpClients::Instance()
{
	static ISTcpClients TcpClients;
	return TcpClients;
}
//-----------------------------------------------------------------------------
void ISTcpClients::Add(qintptr SocketDescriptor, unsigned int UserID, const QString &Address, unsigned short Port)
{
	CRITICAL_SECTION_LOCK(&CriticalSection);
	ISClientInfo ClientInfo;
	ClientInfo.ID = UserID;
	ClientInfo.Address = Address;
	ClientInfo.Port = Port;
	Clients[SocketDescriptor] = ClientInfo;
	CRITICAL_SECTION_UNLOCK(&CriticalSection);
}
//-----------------------------------------------------------------------------
void ISTcpClients::Remove(qintptr SocketDescriptor)
{
	CRITICAL_SECTION_LOCK(&CriticalSection);
	auto It = Clients.find(SocketDescriptor);
	if (It != Clients.end())
	{
		Clients.erase(It->first);
	}
	CRITICAL_SECTION_UNLOCK(&CriticalSection);
}
//-----------------------------------------------------------------------------
ISVectorUInt ISTcpClients::GetClientsID()
{
	ISVectorUInt VectorUInt;
	CRITICAL_SECTION_LOCK(&CriticalSection);
	VectorUInt.resize(Clients.size());
	size_t Index = 0;
	for (const auto &MapItem : Clients)
	{
		VectorUInt[Index] = MapItem.second.ID;
		++Index;
	}
	CRITICAL_SECTION_UNLOCK(&CriticalSection);
	return VectorUInt;
}
//-----------------------------------------------------------------------------
ISClientInfo ISTcpClients::GetClient(unsigned int UserID)
{
	CRITICAL_SECTION_LOCK(&CriticalSection);
	ISClientInfo ClientInfo;
	for (const auto &MapItem : Clients) //Обходим список клиентов
	{
		if (MapItem.second.ID == UserID) //Если нашли нужного - выходим из цикла
		{
			ClientInfo = MapItem.second;
			break;
		}
	}
	CRITICAL_SECTION_UNLOCK(&CriticalSection);
	return ClientInfo;
}
//-----------------------------------------------------------------------------
bool ISTcpClients::ExistUserID(unsigned int UserID)
{
	return ISAlgorithm::VectorContains(GetClientsID(), UserID);
}
//-----------------------------------------------------------------------------
size_t ISTcpClients::GetCount()
{
	CRITICAL_SECTION_LOCK(&CriticalSection);
	size_t Result = Clients.size();
	CRITICAL_SECTION_UNLOCK(&CriticalSection);
	return Result;
}
//-----------------------------------------------------------------------------
void ISTcpClients::UpdateLastQuery(qintptr SocketDescriptor, const QString &MessageType, bool Result, unsigned long long MSec)
{
	CRITICAL_SECTION_LOCK(&CriticalSection);
	auto It = Clients.find(SocketDescriptor);
	if (It != Clients.end())
	{
		Clients[SocketDescriptor].LastQueryDT = QDateTime::currentDateTime();
		Clients[SocketDescriptor].LastQueryType = MessageType;
		Clients[SocketDescriptor].LastQueryResult = Result;
		Clients[SocketDescriptor].LastQueryMSec = MSec;

	}
	CRITICAL_SECTION_UNLOCK(&CriticalSection);
}
//-----------------------------------------------------------------------------
