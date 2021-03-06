#pragma once
#ifndef _ISTCPMESSAGE_H_INCLUDED
#define _ISTCPMESSAGE_H_INCLUDED
//-----------------------------------------------------------------------------
#include "ISTcpSocket.h"
#include "ISNamespace.h"
//-----------------------------------------------------------------------------
class ISTcpMessage
{
public:
	ISTcpMessage(ISTcpSocket *tcp_socket);
	~ISTcpMessage();

	QString GetErrorString() const; //�������� ��������� �������� ������
	void SetErrorString(const QString &error_string); //�������� ��������� �������� ������
	bool IsValid() const; //�������� ���������� ���������

	QString Type; //��� ���������
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
