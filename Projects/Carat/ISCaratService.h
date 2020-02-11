#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISProcessCore.h"
//-----------------------------------------------------------------------------
class ISCaratService : public QObject
{
	Q_OBJECT

signals:
	void QuitLoop();

public:
	ISCaratService(QObject *parent = 0);
	virtual ~ISCaratService();

	void StartService(); //������ �������

protected:
	void CoreStart(ISProcessCore *ProcessCore); //������ ����
	void ReadyReadStandartOutput();
	
	void NewConnection();
	void DisconnectedClient();
	void AppendSocketMessage(const QString &Message);
	void ReadyRead();
	void WriteSocket(QTcpSocket *TcpSocket, const QString &String); //��������� ������ � �����

private:
	QEventLoop *EventLoop;
	QDateTime Uptime;
	QVector<ISProcessCore*> Cores;
	
	QTcpServer *TcpServer;
	QVector<QTcpSocket*> VectorClients;
	QMap<int, QString> Buffer;
};
//-----------------------------------------------------------------------------
