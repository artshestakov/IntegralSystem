#pragma once
#ifndef _ISTCPMESSAGE_H_INCLUDED
#define _ISTCPMESSAGE_H_INCLUDED
//-----------------------------------------------------------------------------
#include "ISTcpSocket.h"
//-----------------------------------------------------------------------------
class ISTcpMessage
{
public:
	ISTcpMessage(ISTcpSocket *tcp_socket);
	~ISTcpMessage();

	QString GetErrorString() const; //�������� ��������� �������� ������
	void SetErrorString(const QString &error_string); //�������� ��������� �������� ������
	bool IsValid() const; //�������� ���������� ���������

	ISNamespace::ApiMessageType Type; //��� ���������
	QString TypeName; //��� ���� ���������
	QVariantMap Parameters; //���������
	ISTcpSocket *TcpSocket; //��������� �� �������

	long long ParseMSec; //����� ��������
	unsigned int Size; //������ ���������
	unsigned int ChunkCount; //���������� ������
	qint64 MessageID; //������������� ���������

private:
	QString ErrorString;
	bool Valid; //���� ���������� ���������
};
//-----------------------------------------------------------------------------
#endif
