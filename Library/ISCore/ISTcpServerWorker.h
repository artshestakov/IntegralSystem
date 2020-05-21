#pragma once
#ifndef _ISTCPSERVERWORKER_H_INCLUDED
#define _ISTCPSERVERWORKER_H_INCLUDED
//-----------------------------------------------------------------------------
#include "ISTcpServerBase.h"
#include "ISTcpAnswer.h"
#include "ISTypedefs.h"
//-----------------------------------------------------------------------------
class ISCORE_EXPORT ISTcpServerWorker : public ISTcpServerBase
{
	Q_OBJECT

public:
	ISTcpServerWorker(QObject *parent = 0);
	virtual ~ISTcpServerWorker();

	bool Run(quint16 Port) override;

protected:
	void incomingConnection(qintptr SocketDescriptor) override; //������� ��������� ����������

private:
	void Disconnected(); //������� ���������� �������
	void ReadyRead(); //������� ������ ������ �� �������
	bool CheckField(const QVariantMap &Parameters, const ISVectorString &Fields, ISTcpAnswer &TcpAnswer); //�������� ���� ���������� �� ����������

private:
	void TestQuery(const QVariantMap &Parameters, ISTcpAnswer &TcpAnswer); //�������� ������
	void Sleep(const QVariantMap &Parameters, ISTcpAnswer &TcpAnswer); //��������
	void ColumnSizer(const QVariantMap &Parameters, ISTcpAnswer &TcpAnswer); //��������� �������� �������

private:
	QTcpSocket *TcpSocket;
	QByteArray Buffer;
	long BufferSize;
	std::map<QString, std::function<void(ISTcpServerWorker &TcpServer, const QVariantMap &, ISTcpAnswer &TcpAnswer)>> Functions;
	std::vector<unsigned char> Token;
};
//-----------------------------------------------------------------------------

#endif
