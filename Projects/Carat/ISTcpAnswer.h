#pragma once
#ifndef _ISTCPANSWER_H_INCLUDED
#define _ISTCPANSWER_H_INCLUDED
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISTcpSocket.h"
//-----------------------------------------------------------------------------
class ISTcpAnswer
{
public:
	ISTcpAnswer(ISTcpSocket *tcp_socket);
	~ISTcpAnswer();

	bool IsError() const;
	QString GetErrorString() const;
	void SetError(const QString &error_string);
	ISTcpSocket* GetSocket();
	qintptr GetSocketDescriptor() const;
	QByteArray ToByteArray() const;

public:
	QVariantMap Parameters;
	
private:
	QString ErrorString;
	bool Error;
	ISTcpSocket *TcpSocket;
	qintptr SocketDescriptor;
};
//-----------------------------------------------------------------------------
#endif
