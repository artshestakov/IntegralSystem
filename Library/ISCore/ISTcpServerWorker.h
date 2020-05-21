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
	void incomingConnection(qintptr SocketDescriptor) override; //Событие входящего соединения

private:
	void Disconnected(); //Событие отключения клиента
	void ReadyRead(); //Событие чтения данных от клиента
	bool CheckField(const QVariantMap &Parameters, const ISVectorString &Fields, ISTcpAnswer &TcpAnswer); //Проверка поля параметров на валидность

private:
	void TestQuery(const QVariantMap &Parameters, ISTcpAnswer &TcpAnswer); //Тестовый запрос
	void Sleep(const QVariantMap &Parameters, ISTcpAnswer &TcpAnswer); //Ожидание
	void ColumnSizer(const QVariantMap &Parameters, ISTcpAnswer &TcpAnswer); //Получение размеров колонок

private:
	QTcpSocket *TcpSocket;
	QByteArray Buffer;
	long BufferSize;
	std::map<QString, std::function<void(ISTcpServerWorker &TcpServer, const QVariantMap &, ISTcpAnswer &TcpAnswer)>> Functions;
	std::vector<unsigned char> Token;
};
//-----------------------------------------------------------------------------

#endif
