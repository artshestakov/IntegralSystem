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
	Clients.emplace(SocketDescriptor, ISClientInfo{ UserID, Address, Port });
	CRITICAL_SECTION_UNLOCK(&CriticalSection);
}
//-----------------------------------------------------------------------------
void ISTcpClients::Remove(qintptr SocketDescriptor)
{
	CRITICAL_SECTION_LOCK(&CriticalSection);
	std::map<qintptr, ISClientInfo>::const_iterator It = Clients.find(SocketDescriptor);
	if (It != Clients.end())
	{
		Clients.erase(It->first);
	}
	CRITICAL_SECTION_UNLOCK(&CriticalSection);
}
//-----------------------------------------------------------------------------
std::vector<ISClientInfo> ISTcpClients::GetClients()
{
	CRITICAL_SECTION_LOCK(&CriticalSection);
	std::vector<ISClientInfo> Vector = ISAlgorithm::ConvertMapToValues(Clients);
	CRITICAL_SECTION_UNLOCK(&CriticalSection);
	return Vector;
}
//-----------------------------------------------------------------------------
