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
	const std::vector<unsigned char> GetToken() const; //Получить токен
	bool IsConnected() const; //Проверить наличие соединения
	bool Reconnect(const QString &Host, quint16 Port, const QString &Login, const QString &Password); //Переподключение
	bool Connect(const QString &Host, quint16 Port, const QString &Login, const QString &Password); //Подключение
	void Disconnect(); //Отключение

private:
	void Timeout();
	void Error(QTcpSocket::SocketError socket_error);
	bool CreateToken(const QString &Login, const QString &Password); //Создание токена
	bool SendToken(); //Отправка токена

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
	std::vector<unsigned char> Token;
	QString PathToken;
};
//-----------------------------------------------------------------------------
#endif
