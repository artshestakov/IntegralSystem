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
	std::vector<qintptr> Descriptors; //Подключенные дескрипторы
	unsigned int WorkerCount; //Количество воркеров
	std::vector<ISTcpWorker *> Workers; //Воркеры
	bool BalancerRunning;
	bool BalancerFinished;

	ISCriticalSection CriticalSection; //Критическая секция для синхронизации
};
//-----------------------------------------------------------------------------
#endif
