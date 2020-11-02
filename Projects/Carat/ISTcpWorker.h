#pragma once
#ifndef _ISTCPWORKER_H_INCLUDED
#define _ISTCPWORKER_H_INCLUDED
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISTcpMessage.h"
#include "ISTcpAnswer.h"
#include "ISTypedefs.h"
//-----------------------------------------------------------------------------
class ISTcpWorker : public QObject
{
	Q_OBJECT

signals:
	void StartedDone();
	void StartedFailed();
	void Answer(ISTcpAnswer *);

public:
	ISTcpWorker(const QString &db_host, int db_port, const QString &db_name, const QString &db_user, const QString &db_password);
	virtual ~ISTcpWorker();

	bool GetStarted() const; //Получить флаг успешного запуска воркера
	bool GetRunning(); //Получить флаг занятости
	void SetMessage(ISTcpMessage *TcpMessage); //Установить сообщение на обработку
	void Run(); //Запуск воркера
	void Stop(); //Остановка воркера

private:
	void Process();
	void Finish(); //Уведомление о завершении работы воркера
	QVariant CheckNullField(const QString &FieldName, const QVariantMap &VariantMap); //Проверка наличия поля
	void Protocol(int UserID, const ISUuid &ActionTypeUID, const QString &TableName, const QString &TableLocalName, const QVariant &ObjectID, const QString &Information);

private:
	bool Auth(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
	bool Sleep(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
	bool GetMetaData(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
	bool Test(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
	bool GetLastClient(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);

private:
    QString ErrorString; //Текстовое описание ошибки запроса
	QString DBConnectionName;
	QString DBHost;
	int DBPort;
	QString DBName;
	QString DBUser;
	QString DBPassword;
	bool IsStarted; //Флаг успешного запуска воркера
	bool IsRunning; //Флаг занятости воркера
	ISTcpMessage *CurrentMessage; //Указатель на текущее сообщение
	bool IsStopped; //Флаг остановки работы воркера

	ISCriticalSection CriticalSection; //Критическая секция для синхронизации
};
//-----------------------------------------------------------------------------
#endif
