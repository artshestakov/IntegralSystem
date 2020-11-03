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
	void Connected(); //Событие подключения к AMI
	void Disconnected(); //Событие отключения от AMI
	void Error(QAbstractSocket::SocketError socket_error); //Событие ошибки
	void ReadyRead(); //Событие чтения входящих данных

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
