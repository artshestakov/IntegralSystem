#pragma once
#ifndef _ISTCPSERVERCARAT_H_INCLUDED
#define _ISTCPSERVERCARAT_H_INCLUDED
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISTcpSocket.h"
#include "ISTcpWorker.h"
//-----------------------------------------------------------------------------
class ISTcpServer : public QTcpServer
{
	Q_OBJECT

public:
	ISTcpServer(quint16 tcp_port, unsigned int worker_count);
	virtual ~ISTcpServer();

	QString GetErrorString() const;
	bool Run();

private:
	void incomingConnection(qintptr SocketDescriptor) override; //������� ��������� ����������
	void ClientDisconnected(); //������� ���������� �������
	void ClientError(QAbstractSocket::SocketError socket_error); //������ �������
	void AcceptError(QTcpSocket::SocketError socket_error); //������ �������� �����������
	
	void QueueBalancerMessage(); //�������� ������� ���������
	void SendAnswer(ISTcpAnswer *TcpAnswer);

private:
	QString ErrorString;
	unsigned short TcpPort;
	unsigned int WorkerCount;
	std::vector<ISTcpWorker *> Workers;
};
//-----------------------------------------------------------------------------
#endif
