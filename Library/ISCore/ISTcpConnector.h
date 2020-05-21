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
	
	QString GetErrorString() const; //�������� ��������� �������� ������
	QTcpSocket* GetSocket(); //�������� ��������� �� �����
	const std::vector<unsigned char> GetToken() const; //�������� �����
	bool IsConnected() const; //��������� ������� ����������
	bool Reconnect(const QString &Host, quint16 Port); //���������������
	bool Connect(const QString &Host, quint16 Port); //�����������
	void Disconnect(); //����������

private:
	void Timeout();
	void Error(QTcpSocket::SocketError socket_error);
	bool CreateToken(); //�������� ������
	bool SendToken(); //�������� ������

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
