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

	void AddMessage(ISTcpMessage *TcpMessage); //�������� ��������� � �������
	ISTcpMessage* GetMessage(); //�������� ��������� ���������

private:
	ISTcpQueue();
	~ISTcpQueue();
	ISTcpQueue(ISTcpQueue const &) {};
	ISTcpQueue& operator=(ISTcpQueue const&) { return *this; };

private:
	QString FilePath; //���� � ����� � ��������� MessageID
	QFile File;
	std::queue<ISTcpMessage *> Queue;
	qint64 MessageID;

	ISCriticalSection CriticalSection; //����������� ������ ��� �������������
};
//-----------------------------------------------------------------------------
#endif
