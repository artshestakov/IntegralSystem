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
	bool Run(); //������ �������
	void Stop(); //��������� �������

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
	bool BalancerRunning;
	bool BalancerFinished;

	//����������� ������ ��� �������������
#ifdef WIN32
	CRITICAL_SECTION CriticalSection;
#else
	pthread_mutex_t CriticalSection;
#endif
};
//-----------------------------------------------------------------------------
#endif
