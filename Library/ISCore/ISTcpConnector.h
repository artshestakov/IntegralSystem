#pragma once
#ifndef _ISTCPCONNECTOR_H_INCLUDED
#define _ISTCPCONNECTOR_H_INCLUDED
//-----------------------------------------------------------------------------
#include "iscore_global.h"
//-----------------------------------------------------------------------------
class ISCORE_EXPORT ISTcpConnector : public QObject
{
	Q_OBJECT

public:
	static ISTcpConnector& Instance();
	
	QString GetErrorString() const; //Получить текствоое описание ошибки
	QTcpSocket* GetSocket(); //Получить указатель на сокет
	bool IsConnected() const; //Проверить наличие соединения
	bool Reconnect(const QString &host, quint16 port); //Переподключение
	bool Connect(const QString &host, quint16 port); //Подключение
	void Disconnect(); //Отключение

private:
	void Timeout();
	void Error(QTcpSocket::SocketError socket_error);
	void StateChanged(QTcpSocket::SocketState socket_state);

private:
	ISTcpConnector();
	~ISTcpConnector();
	ISTcpConnector(ISTcpConnector const &) {};
	ISTcpConnector& operator=(ISTcpConnector const&) { return *this; };

private:
	QString ErrorString;
	QTcpSocket *TcpSocket;
	QEventLoop EventLoop;
	QTimer *Timer;
};
//-----------------------------------------------------------------------------
#endif
