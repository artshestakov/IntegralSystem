#pragma once
#ifndef _ISTCPSOCKET_H_INCLUDED
#define _ISTCPSOCKET_H_INCLUDED
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISNamespace.h"
//-----------------------------------------------------------------------------
class ISTcpSocket : public QTcpSocket
{
	Q_OBJECT

public:
	ISTcpSocket(qintptr SocketDescriptor, QObject *parent = 0);
	virtual ~ISTcpSocket();

private:
	void ReadyRead(); //������� �������� ������
	void SendErrorQuery(const QString &ErrorString); //�������� ������ �������
	ISNamespace::ApiMessageType GetMessageType(const QString &TypeName) const; //�������� ��� ��������� �� ��� �����

private:
	QByteArray Buffer;
};
//-----------------------------------------------------------------------------
#endif
