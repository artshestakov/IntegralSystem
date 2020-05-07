#pragma once
#ifndef _ISTCPSERVERWORKER_H_INCLUDED
#define _ISTCPSERVERWORKER_H_INCLUDED
//-----------------------------------------------------------------------------
#include "ISTcpServerBase.h"
#include "ISTcpApi.h"
//-----------------------------------------------------------------------------
class ISCORE_EXPORT ISTcpServerWorker : public ISTcpServerBase
{
	Q_OBJECT

public:
	ISTcpServerWorker(QObject *parent = 0);
	virtual ~ISTcpServerWorker();

protected:
	void incomingConnection(qintptr SocketDescriptor) override; //������� ��������� ����������

private:
	void ReadyRead(); //������� ������ ������ �� �������
	void Disconnected(); //������� ���������� �������

private:
	ISTcpApi *TcpApi;
	QTcpSocket *TcpSocket;
	QByteArray Buffer;
};
//-----------------------------------------------------------------------------

#endif
