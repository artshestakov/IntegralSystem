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

	void SetUserGroupID(int user_group_id);
	int GetUserGroupID() const;

	void SetUserIsSystem(bool user_is_system);
	bool GetUserIsSystem() const;

private:
	void ReadyRead(); //������� �������� ������
	void Error(QAbstractSocket::SocketError socket_error); //������� ������ ������
	ISNamespace::ApiMessageType GetMessageType(const QString &TypeName) const; //�������� ��� ��������� �� ��� �����
	void ClearBuffer(); //������� ������
	void Timeout(); //������� ��������

private:
	QByteArray Buffer; //�����
	unsigned int MessageSize; //������ ���������
	unsigned int ChunkCount; //���������� ������ ���������
	QTimer *Timer;

	bool IsAuthorized; //���� ����������� �������
	int UserID; //������������� ������������
	int UserGroupID; //������������� ������ ������������
	bool UserIsSystem; //���� ���������� ������������
};
//-----------------------------------------------------------------------------
#endif
