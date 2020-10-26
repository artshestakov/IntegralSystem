#pragma once
#ifndef _ISCARATCONTROLLER_H_INCLUDED
#define _ISCARATCONTROLLER_H_INCLUDED
//-----------------------------------------------------------------------------
#include "StdAfx.h"
//-----------------------------------------------------------------------------
class ISCaratController : public QThread
{
	Q_OBJECT

signals:
	void Started();
	void Shutdown();

public:
	ISCaratController(QObject *parent);
	virtual ~ISCaratController();

	QString GetErrorString() const; //�������� ��������� �������� ������
	bool Start(); //������ ������
	void Stop(); //��������� ������

protected:
	void run() override;

private:
	void Send(QTcpSocket *TcpSocket, const QByteArray &Responce);
	void Wait();

private:
	QString ErrorString;
	unsigned short ControllerPort;
	bool IsRunning;
	bool IsFinished;

	//����������� ������ ��� �������������
#ifdef WIN32
	CRITICAL_SECTION CriticalSection;
#else
	pthread_mutex_t CriticalSection;
#endif
};
//-----------------------------------------------------------------------------
#endif
