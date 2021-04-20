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
    void Reconnect(); //������ � ������������� ���������������

public:
    static ISTcpConnector& Instance();

    QString GetErrorString() const; //�������� ��������� �������� ������
    QTcpSocket* GetSocket(); //�������� ��������� �� �����
    bool IsConnected() const; //��������� ������� ����������
    bool Connect(const QString &Host, quint16 Port); //�����������
    void Disconnect(); //����������

private:
    void Error(QTcpSocket::SocketError socket_error); //������� ������ ������

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
