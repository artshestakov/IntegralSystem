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

	void Add(qintptr SocketDescriptor, unsigned int UserID, const QString &Address, unsigned short Port); //�������� �������
	void Remove(qintptr SocketDescriptor); //������� �������
	ISVectorUInt GetClientsID(); //�������� ������ ��������������� ��������
	ISClientInfo GetClient(unsigned int UserID); //�������� ���������� � ������� �� ��� ��������������
	bool ExistUserID(unsigned int UserID); //��������� ������� ������ ������������
	size_t GetCount(); //�������� ���������� ��������

	void UpdateLastQuery(qintptr SocketDescriptor, ISNamespace::ApiMessageType MessageType, bool Result, unsigned long long MSec); //�������� ���� � ��������� ���������� �������

private:
	ISTcpClients();
	~ISTcpClients();
	ISTcpClients(const ISTcpClients&) = delete;
	ISTcpClients(ISTcpClients&&) = delete;
	ISTcpClients& operator=(const ISTcpClients&) = delete;
	ISTcpClients& operator=(ISTcpClients&&) = delete;

private:
	std::map<qintptr, ISClientInfo> Clients;
	ISCriticalSection CriticalSection;
};
//-----------------------------------------------------------------------------
#endif
