#pragma once
#ifndef _ISCARATSERVICE_H_INCLUDED
#define _ISCARATSERVICE_H_INCLUDED
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISTcpServerCarat.h"
//-----------------------------------------------------------------------------
class ISCaratService : public QObject
{
	Q_OBJECT

public:
	ISCaratService(QObject *parent = 0);
	virtual ~ISCaratService();

	bool StartService(); //Запуск сервера

private:
	void NewConnection(); //Событие подключения нового клиента в режиме отладки
	void Disconnected(); //Событие отключения клиента в режиме отладки

private:
	QLocalServer *LocalServer;
	QLocalSocket *LocalSocket;
	bool IsConnectedDebugger;
	ISTcpServerCarat *TcpServer;
};
//-----------------------------------------------------------------------------
#endif
