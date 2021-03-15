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

	void Shutdown(); //��������� ������ � ��������

	void AddMessage(ISTcpMessage *TcpMessage); //�������� ��������� � �������
	ISTcpMessage* GetMessage(); //�������� ��������� ���������

	void AddAnswer(ISTcpAnswer *TcpAnswer); //�������� ����� � �������
	ISTcpAnswer* GetAnswer(); //�������� ��������� �����

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
	bool IsActive; //���� ���������� �������

	//����������� ������
	ISCriticalSection CriticalSectionM;
	ISCriticalSection CriticalSectionA;
};
//-----------------------------------------------------------------------------
#endif
