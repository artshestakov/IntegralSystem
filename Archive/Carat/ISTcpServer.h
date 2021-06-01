#pragma once
#ifndef _ISTCPSERVER_H_INCLUDED
#define _ISTCPSERVER_H_INCLUDED
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISTcpWorker.h"
#include "ISTypedefs.h"
#include "ISStructs.h"
//-----------------------------------------------------------------------------
class ISTcpServer : public QTcpServer
{
	Q_OBJECT

public:
	ISTcpServer(QObject *parent);
	virtual ~ISTcpServer();

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
	std::vector<qintptr> Descriptors; //������������ �����������
	unsigned int WorkerCount; //���������� ��������
	std::vector<ISTcpWorker *> Workers; //�������
	bool BalancerRunning;
	bool BalancerFinished;

	ISCriticalSection CriticalSection; //����������� ������ ��� �������������
};
//-----------------------------------------------------------------------------
#endif
