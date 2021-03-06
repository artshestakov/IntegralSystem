#pragma once
#ifndef _ISTCPSOCKET_H_INCLUDED
#define _ISTCPSOCKET_H_INCLUDED
//-----------------------------------------------------------------------------
#include "StdAfx.h"
//-----------------------------------------------------------------------------
class ISTcpSocket : public QTcpSocket
{
	Q_OBJECT

public:
	ISTcpSocket(qintptr socket_descriptor, QObject *parent = 0);
	virtual ~ISTcpSocket();

	qintptr GetSocketDescriptor() const;
	QString GetAddress() const;

	void SetAuthorized(bool authorized);
	bool GetAuthorized() const;

	void SetUserID(unsigned int user_id);
	unsigned int GetUserID() const;

	void SetUserIsSystem(bool user_is_system);
	bool GetUserIsSystem() const;

private:
	void ReadyRead(); //������� �������� ������
	void Error(QAbstractSocket::SocketError socket_error); //������� ������ ������
	void ClearBuffer(); //������� ������
	void Timeout(); //������� ��������

private:
	qintptr SocketDescriptor;
	QString Address;
	QByteArray Buffer; //�����
	unsigned int MessageSize; //������ ���������
	unsigned int ChunkCount; //���������� ������ ���������
	QTimer *Timer;

	bool IsAuthorized; //���� ����������� �������
	unsigned int UserID; //������������� ������������
	bool UserIsSystem; //���� ���������� ������������
};
//-----------------------------------------------------------------------------
#endif
