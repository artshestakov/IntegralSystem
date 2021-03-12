#pragma once
#ifndef _ISTCPQUEUE_H_INCLUDED
#define _ISTCPQUEUE_H_INCLUDED
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISTcpMessage.h"
#include "ISTypedefs.h"
//-----------------------------------------------------------------------------
class ISTcpQueue
{
public:
	static ISTcpQueue& Instance();

	void Shutdown(); //Остановка работы с очередью
	void AddMessage(ISTcpMessage *TcpMessage); //Добавить сообщение в очередь
	ISTcpMessage* GetMessage(); //Получить очередное сообщение

private:
	ISTcpQueue();
	~ISTcpQueue();
	ISTcpQueue(const ISTcpQueue&) = delete;
	ISTcpQueue(ISTcpQueue&&) = delete;
	ISTcpQueue& operator=(const ISTcpQueue&) = delete;
	ISTcpQueue& operator=(ISTcpQueue&&) = delete;

private:
	std::queue<ISTcpMessage *> Queue;
	bool IsActive; //Флаг активности очереди
	ISCriticalSection CriticalSection; //Критическая секция для синхронизации
};
//-----------------------------------------------------------------------------
#endif
