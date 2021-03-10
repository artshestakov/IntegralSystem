#pragma once
#ifndef _ISTCPSERVER_H_INCLUDED
#define _ISTCPSERVER_H_INCLUDED
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISTypedefs.h"
//-----------------------------------------------------------------------------
class ISTcpServer
{
public:
	static ISTcpServer& Instance();

	std::string GetErrorString() const;
	
	bool Start();

private:
	void WorkerAcceptor();
	void ReadData();

private:
	ISTcpServer();
	~ISTcpServer();
	ISTcpServer(const ISTcpServer&) = delete;
	ISTcpServer(ISTcpServer&&) = delete;
	ISTcpServer& operator=(const ISTcpServer&) = delete;
	ISTcpServer& operator=(ISTcpServer&&) = delete;

private:
	std::string ErrorString;
	SOCKET Socket;
	ISVectorSocket Clients;
};
//-----------------------------------------------------------------------------
#endif
