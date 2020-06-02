#pragma once
#ifndef _ISTCPSERVERCARAT_H_INCLUDED
#define _ISTCPSERVERCARAT_H_INCLUDED
//-----------------------------------------------------------------------------
#include "ISTcpServerBase.h"
//-----------------------------------------------------------------------------
class ISCORE_EXPORT ISTcpServerCarat : public ISTcpServerBase
{
	Q_OBJECT

public:
	ISTcpServerCarat(QObject *parent = 0);
	virtual ~ISTcpServerCarat();

	bool Run(quint16 Port) override;

protected:
	void incomingConnection(qintptr SocketDescriptor) override; //������� ��������� ����������
	void Disconnected(); //������� ���������� �������
    virtual bool StartWorker(const QString &Port, const QString &Login, const QString &Password);

private:
	QLocalServer *ServerController;
	QString DBHost;
	int DBPort;
	QString DBName;
	bool IsDisconnected;
};
//-----------------------------------------------------------------------------
#endif
