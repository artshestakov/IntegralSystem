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
	void SetDBHost(const QString &db_host);
	void SetDBPort(int db_port);
	void SetDBName(const QString &db_name);

protected:
	void incomingConnection(qintptr SocketDescriptor) override; //Событие входящего соединения
	void Disconnected(); //Событие отключения клиента
	virtual bool StartWorker(QTcpSocket *TcpSocket, const QString &Port, const QString &Login, const QString &Password, const QString &Key);

private:
	bool InitCrypter(); //Инициализация криптера

private:
	QLocalServer *ServerController;
	QString DBHost;
	int DBPort;
	QString DBName;
	bool IsDisconnected;

	HINSTANCE HModuleCrypter;
	typedef const char *(__stdcall *DecryptMessage)(const char *);
	typedef const char *(__stdcall *GetError)(void);
	DecryptMessage decrypt_message;
	GetError get_error;
};
//-----------------------------------------------------------------------------
#endif
