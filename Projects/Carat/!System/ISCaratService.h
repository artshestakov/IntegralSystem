#pragma once
//-----------------------------------------------------------------------------
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

	void StartService(); //Запуск сервера

protected:
	void CoreStart(ISProcessCore *ProcessCore); //Запуск ядра
	void ReadyReadStandartOutput();
	void CrashedCore(const QString &CoreName); //Падение ядра
	
	void NewConnection();
	void DisconnectedClient();
	void AppendSocketMessage(const QString &Message);
	void ReadyRead();
	void WriteSocket(QTcpSocket *TcpSocket, const QString &String); //Отправить данные в сокет

private slots:
	void Help(QTcpSocket *TcpSocket); //Помощь
	void Exit(QTcpSocket *TcpSocket); //Отключение
	void Quit(QTcpSocket *TcpSocket); //Отключение
	void Close(QTcpSocket *TcpSocket); //Отключение
	void Disconnect(QTcpSocket *TcpSocket); //Отключение
	void Status(QTcpSocket *TcpSocket); //Статус ядер

private:
	QEventLoop *EventLoop;
	QDateTime Uptime;
	QVector<ISProcessCore*> Cores;
	
	QTcpServer *TcpServer;
	QVector<QTcpSocket*> VectorClients;
	QMap<int, QString> Buffer;
};
//-----------------------------------------------------------------------------
