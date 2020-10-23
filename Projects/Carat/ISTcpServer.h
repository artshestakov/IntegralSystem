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
	ISTcpServer(QObject *parent);
	virtual ~ISTcpServer();

	QString GetErrorString() const;
	unsigned int GetWorkerCount() const;
	bool Run();

private:
	void incomingConnection(qintptr SocketDescriptor) override; //������� ��������� ����������
	void ClientDisconnected(); //������� ���������� �������
	void AcceptError(QTcpSocket::SocketError socket_error); //������ �������� �����������
	
	void QueueBalancerMessage(); //�������� ������� ���������
	void SendAnswer(ISTcpAnswer *TcpAnswer);

private:
	QString ErrorString;
	unsigned int WorkerCount;
	std::vector<ISTcpWorker *> Workers;
};
//-----------------------------------------------------------------------------
#endif
