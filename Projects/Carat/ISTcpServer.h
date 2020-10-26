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
	bool Run(); //Запуск сервера
	void Stop(); //Остановка сервера

private:
	void incomingConnection(qintptr SocketDescriptor) override; //Событие входящего соединения
	void ClientDisconnected(); //Событие отключения клиента
	void AcceptError(QTcpSocket::SocketError socket_error); //Ошибка принятия подключения
	
	void QueueBalancerMessage(); //Балансер очереди сообщений
	void SendAnswer(ISTcpAnswer *TcpAnswer);

private:
	QString ErrorString;
	unsigned int WorkerCount;
	std::vector<ISTcpWorker *> Workers;
	bool BalancerRunning;
	bool BalancerFinished;

	//Критическая секция для синхронизации
#ifdef WIN32
	CRITICAL_SECTION CriticalSection;
#else
	pthread_mutex_t CriticalSection;
#endif
};
//-----------------------------------------------------------------------------
#endif
