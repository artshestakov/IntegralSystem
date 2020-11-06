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
	void RemoteHostClose(); //������ �������� ���������� ��������

public:
	static ISTcpConnector& Instance();
	
	QString GetErrorString() const; //�������� ��������� �������� ������
	QTcpSocket* GetSocket(); //�������� ��������� �� �����
	bool IsConnected() const; //��������� ������� ����������
	bool Connect(const QString &Host, quint16 Port); //�����������
	void Disconnect(); //����������

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
