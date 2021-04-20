#pragma once
#ifndef _ISTCPCONNECTOR_H_INCLUDED
#define _ISTCPCONNECTOR_H_INCLUDED
//-----------------------------------------------------------------------------
#include "isgui_global.h"
//-----------------------------------------------------------------------------
class ISTcpConnector : public QObject
{
    Q_OBJECT

signals:
    void Reconnect(); //Сигнал о необходимости переподключения

public:
    static ISTcpConnector& Instance();

    QString GetErrorString() const; //Получить текствоое описание ошибки
    QTcpSocket* GetSocket(); //Получить указатель на сокет
    bool IsConnected() const; //Проверить наличие соединения
    bool Connect(const QString &Host, quint16 Port); //Подключение
    void Disconnect(); //Отключение

private:
    void Error(QTcpSocket::SocketError socket_error); //Событие ошибки сокета

private:
    ISTcpConnector();
    ~ISTcpConnector();
    ISTcpConnector(const ISTcpConnector &) = delete;
    ISTcpConnector(ISTcpConnector &&) = delete;
    ISTcpConnector& operator=(const ISTcpConnector &) = delete;
    ISTcpConnector& operator=(ISTcpConnector &&) = delete;

private:
    QString ErrorString;
    QTcpSocket *TcpSocket;
    bool HandlingError;
};
//-----------------------------------------------------------------------------
#endif
