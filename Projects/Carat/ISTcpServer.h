#pragma once
#ifndef _ISTCPSERVERCARAT_H_INCLUDED
#define _ISTCPSERVERCARAT_H_INCLUDED
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISTcpSocket.h"
#include "ISTcpWorker.h"
#include "ISTypedefs.h"
//-----------------------------------------------------------------------------
class ISTcpServer : public QTcpServer
{
	Q_OBJECT

public:
	ISTcpServer(QObject *parent);
	virtual ~ISTcpServer();

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

	ISCriticalSection CriticalSection; //Критическая секция для синхронизации
};
//-----------------------------------------------------------------------------
#endif
