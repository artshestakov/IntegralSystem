#pragma once
#ifndef _ISTCPSOCKET_H_INCLUDED
#define _ISTCPSOCKET_H_INCLUDED
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISNamespace.h"
//-----------------------------------------------------------------------------
class ISTcpSocket : public QTcpSocket
{
	Q_OBJECT

public:
	ISTcpSocket(qintptr SocketDescriptor, QObject *parent = 0);
	virtual ~ISTcpSocket();

	void SetAuthorized(bool authorized);
	bool GetAuthorized() const;

	void SetUserID(int user_id);
	int GetUserID() const;

private:
	void ReadyRead(); //������� �������� ������
	void Error(QAbstractSocket::SocketError socket_error); //������� ������ ������
	ISNamespace::ApiMessageType GetMessageType(const QString &TypeName) const; //�������� ��� ��������� �� ��� �����
	void ClearBuffer(); //������� ������

private:
	QByteArray Buffer; //�����
	int MessageSize; //������ ���������
	int ChunkCount; //���������� ������ ���������
	bool IsAuthorized; //���� ����������� �������
	int UserID; //������������� ������������
	QTimer *Timer;
};
//-----------------------------------------------------------------------------
#endif
