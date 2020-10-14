#pragma once
#ifndef _ISTCPQUEUE_H_INCLUDED
#define _ISTCPQUEUE_H_INCLUDED
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISStructs.h"
//-----------------------------------------------------------------------------
class ISTcpQueue
{
public:
	static ISTcpQueue& Instance();

	void AddMessage(ISTcpMessage *TcpMessage); //�������� ��������� � �������
	ISTcpMessage* GetMessage(); //�������� ��������� ���������

private:
	ISTcpQueue();
	~ISTcpQueue();
	ISTcpQueue(ISTcpQueue const &) {};
	ISTcpQueue& operator=(ISTcpQueue const&) { return *this; };

private:
	std::queue<ISTcpMessage *> Queue;

	//����������� ������ ��� �������������
#ifdef WIN32
	CRITICAL_SECTION CriticalSection; 
#else
	pthread_mutex_t CriticalSection;
#endif
};
//-----------------------------------------------------------------------------
#endif
