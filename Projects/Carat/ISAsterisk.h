#pragma once
#ifndef _ISASTERISK_H_INCLUDED
#define _ISASTERISK_H_INCLUDED
//-----------------------------------------------------------------------------
#include "iscore_global.h"
//-----------------------------------------------------------------------------
class ISAsterisk : public QThread
{
	Q_OBJECT

public:
	ISAsterisk(QObject *parent);
	virtual ~ISAsterisk();

	bool Start();

protected:
	void run() override;

private:
	void Connected(); //������� ����������� � AMI
	void Disconnected(); //������� ���������� �� AMI
	void Error(QAbstractSocket::SocketError socket_error); //������� ������
	void ReadyRead(); //������� ������ �������� ������

private:
	QTcpSocket *TcpSocket;
	QByteArray Buffer;

	QString Host;
	unsigned short Port;
	QString Login;
	QString Password;
};
//-----------------------------------------------------------------------------
#endif
