#pragma once
#ifndef _ISTCPSERVERCARAT_H_INCLUDED
#define _ISTCPSERVERCARAT_H_INCLUDED
//-----------------------------------------------------------------------------
#include "iscore_global.h"
//-----------------------------------------------------------------------------
class ISCORE_EXPORT ISTcpServer : public QTcpServer
{
	Q_OBJECT

public:
	ISTcpServer(QObject *parent = 0);
	virtual ~ISTcpServer();

	QString GetErrorString() const;
	bool Run(quint16 Port);

private:
	void incomingConnection(qintptr SocketDescriptor) override; //������� ��������� ����������
	void ClientDisconnected(); //������� ���������� �������
	void ClientError(QAbstractSocket::SocketError socket_error); //������ �������
	void AcceptError(QTcpSocket::SocketError socket_error); //������ �������� �����������

	void Send(QTcpSocket *TcpSocket, const QVariantMap &Data); //�������� ������
	void SendError(QTcpSocket *TcpSocket, const QString &error_string); //�������� ������

private:
	QString ErrorString;
	QString DBHost;
	int DBPort;
	QString DBName;
	std::vector<QTcpSocket*> Clients;
};
//-----------------------------------------------------------------------------
#endif
