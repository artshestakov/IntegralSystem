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
	virtual bool StartWorker(QTcpSocket *TcpSocket, const QString &Port, const QString &Login, const QString &Password);

private:
	bool WaitToken(QTcpSocket *TcpSocket, QByteArray &Buffer); //Ожидание токена
	bool InitCrypter(); //Инициализация криптера

private:
	QLocalServer *ServerController;
	QString DBHost;
	int DBPort;
	QString DBName;
	bool IsDisconnected;

	typedef const char *(__stdcall *DecryptMessage)(const char *); //Определение функции дешифрования изображения
	typedef const char *(__stdcall *GetError)(void); //Определение функции получения ошибки при дешифровании изображения
	HINSTANCE HModuleCrypter; //Модуль криптера
	DecryptMessage decrypt_message; //Указатель на функцию дешифрования изображения
	GetError get_error; //Указатель на функцию получения ошибки при дешифровании изображения
};
//-----------------------------------------------------------------------------
#endif
