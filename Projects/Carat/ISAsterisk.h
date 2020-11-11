#pragma once
#ifndef _ISASTERISK_H_INCLUDED
#define _ISASTERISK_H_INCLUDED
//-----------------------------------------------------------------------------
#include "iscore_global.h"
#include "ISTypedefs.h"
#include "ISQuery.h"
//-----------------------------------------------------------------------------
class ISAsterisk : public QThread
{
	Q_OBJECT

public:
	ISAsterisk(QObject *parent);
	virtual ~ISAsterisk();

	bool Start();

protected:
	void run() override;

private:
	void Connect();
	void Connected(); //������� ����������� � AMI
	void Disconnected(); //������� ���������� �� AMI
	void Error(QAbstractSocket::SocketError socket_error); //������� ������
	void ReadyRead(); //������� ������ �������� ������
	void SendAction(const QString &ActionType, const ISStringMap &StringMap);
	const QString ExtractOfPackage(const ISStringMap &AMIPackage, const QString &Key) const;

	void ActionLogin(); //�����������

	void ResponseLogin(const ISStringMap &AMIPackage);

	void EventCDR(const ISStringMap &AMIPackage); //������� ����������

private:
	QString Buffer;
	int Pos;
	bool IsFirstPackage;
	QTcpSocket *TcpSocket;

	QString Host;
	unsigned short Port;
	QString Login;
	QString Password;

	ISQuery *qInsert;
};
//-----------------------------------------------------------------------------
#endif
