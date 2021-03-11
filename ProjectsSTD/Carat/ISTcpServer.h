#pragma once
#ifndef _ISTCPSERVER_H_INCLUDED
#define _ISTCPSERVER_H_INCLUDED
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISTypedefs.h"
#include "ISStructs.h"
//-----------------------------------------------------------------------------
class ISTcpServer
{
public:
	static ISTcpServer& Instance();

	std::string GetErrorString() const;
	
	bool Start();

private:
	void WorkerAcceptor();
	void ReadData(const ISTcpClient &TcpClient);
	void CloseSocket(SOCKET Socket);
	void ClientAdd(const ISTcpClient &TcpClient);

private:
	ISTcpServer();
	~ISTcpServer();
	ISTcpServer(const ISTcpServer&) = delete;
	ISTcpServer(ISTcpServer&&) = delete;
	ISTcpServer& operator=(const ISTcpServer&) = delete;
	ISTcpServer& operator=(ISTcpServer&&) = delete;

private:
	std::string ErrorString;
	SOCKET SocketServer;
	std::vector<ISTcpClient> Clients;
	ISCriticalSection CriticalSection;
};
//-----------------------------------------------------------------------------
#endif
