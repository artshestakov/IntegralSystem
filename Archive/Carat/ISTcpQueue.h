#pragma once
#ifndef _ISTCPQUEUE_H_INCLUDED
#define _ISTCPQUEUE_H_INCLUDED
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISTcpMessage.h"
#include "ISTcpAnswer.h"
#include "ISTypedefs.h"
//-----------------------------------------------------------------------------
class ISTcpQueue
{
public:
	static ISTcpQueue& Instance();

	void ReadMessageID(); //������ ��������������
	void WriteMessageID(); //���������� �������������� � ����

	void Shutdown(); //��������� ������ � ��������
	void AddMessage(ISTcpMessage *TcpMessage); //�������� ��������� � �������
	ISTcpMessage* GetMessage(); //�������� ��������� ���������

private:
	ISTcpQueue();
	~ISTcpQueue();
	ISTcpQueue(const ISTcpQueue&) = delete;
	ISTcpQueue(ISTcpQueue&&) = delete;
	ISTcpQueue& operator=(const ISTcpQueue&) = delete;
	ISTcpQueue& operator=(ISTcpQueue&&) = delete;

private:
	std::queue<ISTcpMessage *> Queue;
	qint64 MessageID;
	bool IsActive; //���� ���������� �������

	ISCriticalSection CriticalSection; //����������� ������ ��� �������������
};
//-----------------------------------------------------------------------------
#endif
