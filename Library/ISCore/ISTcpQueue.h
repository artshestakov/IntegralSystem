#pragma once
#ifndef _ISTCPQUEUE_H_INCLUDED
#define _ISTCPQUEUE_H_INCLUDED
//-----------------------------------------------------------------------------
#include "iscore_global.h"
#include "ISStructs.h"
//-----------------------------------------------------------------------------
class ISCORE_EXPORT ISTcpQueue
{
public:
	static ISTcpQueue& Instance();

	void AddMessage(ISTcpMessage *TcpMessage); //Добавить сообщение в очередь
	ISTcpMessage* GetMessage(); //Получить очередное сообщение

private:
	ISTcpQueue();
	~ISTcpQueue();
	ISTcpQueue(ISTcpQueue const &) {};
	ISTcpQueue& operator=(ISTcpQueue const&) { return *this; };

private:
	std::queue<ISTcpMessage *> Queue;

	//Критическая секция для синхронизации
#ifdef WIN32
	CRITICAL_SECTION CriticalSection; 
#else
	pthread_mutex_t CriticalSection;
#endif
};
//-----------------------------------------------------------------------------
#endif
