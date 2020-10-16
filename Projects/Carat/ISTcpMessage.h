#pragma once
#ifndef _ISTCPMESSAGE_H_INCLUDED
#define _ISTCPMESSAGE_H_INCLUDED
//-----------------------------------------------------------------------------
#include "ISTcpSocket.h"
//-----------------------------------------------------------------------------
class ISTcpMessage
{
public:
	ISTcpMessage(ISNamespace::ApiMessageType type, const QVariantMap &parameters, ISTcpSocket *tcp_socket);
	ISTcpMessage(ISTcpSocket *tcp_socket, const QString &error_string);
	~ISTcpMessage();

	QString GetErrorString() const; //�������� ��������� �������� ������
	bool IsValid() const; //�������� ���������� ���������

	ISNamespace::ApiMessageType Type; //��� ���������
	QVariantMap Parameters; //���������
	ISTcpSocket *TcpSocket; //��������� �� �������

private:
	QString ErrorString;
	bool Valid; //���� ���������� ���������
};
//-----------------------------------------------------------------------------
#endif
