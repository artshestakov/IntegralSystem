#pragma once
#ifndef _ISTCPCONNECTOR_H_INCLUDED
#define _ISTCPCONNECTOR_H_INCLUDED
//-----------------------------------------------------------------------------
#include "iscore_global.h"
//-----------------------------------------------------------------------------
class ISCORE_EXPORT ISTcpConnector : public QObject
{
	Q_OBJECT

signals:
	void RemoteHostClose(); //Сигнал закрытия соединения сервером

public:
	static ISTcpConnector& Instance();
	
	QString GetErrorString() const; //Получить текствоое описание ошибки
	QTcpSocket* GetSocket(); //Получить указатель на сокет
	bool IsConnected() const; //Проверить наличие соединения
	bool Connect(const QString &Host, quint16 Port); //Подключение
	void Disconnect(); //Отключение

private:
	void StateChanged(QAbstractSocket::SocketState socket_state);
	void Error(QTcpSocket::SocketError socket_error);

private:
	ISTcpConnector();
	~ISTcpConnector();
    ISTcpConnector(ISTcpConnector const &) : QObject() { };
	ISTcpConnector& operator=(ISTcpConnector const&) { return *this; };

private:
	QString ErrorString;
	QTcpSocket *TcpSocket;
};
//-----------------------------------------------------------------------------
#endif
