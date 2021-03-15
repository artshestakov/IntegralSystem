#pragma once
#ifndef _ISTCPQUEUE_H_INCLUDED
#define _ISTCPQUEUE_H_INCLUDED
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISTcpMessage.h"
#include "ISTypedefs.h"
#include "ISTcpAnswer.h"
//-----------------------------------------------------------------------------
class ISTcpQueue
{
public:
	static ISTcpQueue& Instance();

	void Shutdown(); //Остановка работы с очередью

	void AddMessage(ISTcpMessage *TcpMessage); //Добавить сообщение в очередь
	ISTcpMessage* GetMessage(); //Получить очередное сообщение

	void AddAnswer(ISTcpAnswer *TcpAnswer); //Добавить ответ в очередь
	ISTcpAnswer* GetAnswer(); //Получить очередной ответ

private:
	ISTcpQueue();
	~ISTcpQueue();
	ISTcpQueue(const ISTcpQueue&) = delete;
	ISTcpQueue(ISTcpQueue&&) = delete;
	ISTcpQueue& operator=(const ISTcpQueue&) = delete;
	ISTcpQueue& operator=(ISTcpQueue&&) = delete;

private:
	std::queue<ISTcpMessage *> QueueM;
	std::queue<ISTcpAnswer *> QueueA;
	bool IsActive; //Флаг активности очереди

	//Критические секции
	ISCriticalSection CriticalSectionM;
	ISCriticalSection CriticalSectionA;
};
//-----------------------------------------------------------------------------
#endif
