#pragma once
#ifndef _ISTCPCLIENT_H_INCLUDED
#define _ISTCPCLIENT_H_INCLUDED
//-----------------------------------------------------------------------------
#include "ISTypedefs.h"
//-----------------------------------------------------------------------------
class ISTcpClient
{
public:
	ISTcpClient();
	~ISTcpClient();

	void AddData(const char *Data, int Size);

public:
	SOCKET Socket;
	std::string IPAddress;
	ISTcpPort Port;

	std::vector<char> Buffer;
	int BufferSize;
};
//-----------------------------------------------------------------------------
#endif
