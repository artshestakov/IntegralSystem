#pragma once
#ifndef _ISTCPSOCKET_H_INCLUDED
#define _ISTCPSOCKET_H_INCLUDED
//-----------------------------------------------------------------------------
#include "iscore_global.h"
//-----------------------------------------------------------------------------
class ISTcpSocket : public QTcpSocket
{
	Q_OBJECT

public:
	ISTcpSocket(qintptr SocketDescriptor, QObject *parent = 0);
	virtual ~ISTcpSocket();

private:
	void ReadyRead(); //������� �������� ������
	void ClearBuffer(); //������� ������
	void Send(const QVariantMap &Data); //�������� ������
	void SendError(const QString &error_string); //�������� ������

private:
	void Auth(const QVariantMap &VariantMap);

private:
	QByteArray Buffer;
	bool IsAuthorized;

private:
	typedef void(ISTcpSocket::*ApiFunction)(const QVariantMap &);
	std::map<QString, ApiFunction> Functions;
};
//-----------------------------------------------------------------------------
#endif
