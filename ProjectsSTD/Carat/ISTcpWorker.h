#pragma once
#ifndef _ISTCPWORKER_H_INCLUDED
#define _ISTCPWORKER_H_INCLUDED
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISTypedefs.h"
#include "ISTcpMessage.h"
#include "ISTcpAnswer.h"
//-----------------------------------------------------------------------------
class ISTcpWorker
{
public:
	ISTcpWorker();
	~ISTcpWorker();

	bool GetBusy(); //�������� "���������" �������
	void SetMessage(ISTcpMessage *TcpMessage); //���������� ��������� �� ���������
	void Start(); //��������� ������
	void Shutdown(); //���������� ������

private:
	void Process();
	bool Execute(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
	bool CheckIsNull(ISTcpMessage *TcpMessage, const std::string &ParameterName);

private:
	bool Auth(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
	bool Sleep(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);

private:
	std::string ErrorString;
	bool IsBusy;
	bool IsRunning;
	bool IsFinished;
	ISTcpMessage *CurrentMessage;
	ISCriticalSection CriticalSection;

	//������ TCP-�������
	typedef bool (ISTcpWorker::*TCPFunction)(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
	std::map<std::string, TCPFunction> MapFunction;
};
//-----------------------------------------------------------------------------
#endif
