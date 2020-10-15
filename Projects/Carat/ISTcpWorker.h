#pragma once
#ifndef _ISTCPWORKER_H_INCLUDED
#define _ISTCPWORKER_H_INCLUDED
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISStructs.h"
//-----------------------------------------------------------------------------
class ISTcpWorker : public QObject
{
	Q_OBJECT

signals:
	void Started();

public:
	ISTcpWorker(const QString &db_host, int db_port, const QString &db_name, const QString &db_user, const QString &db_password);
	virtual ~ISTcpWorker();

	bool GetRunning(); //Получить флаг занятости
	void SetMessage(ISTcpMessage *TcpMessage); //Установить сообщение на обработку
	void Run(); //Запуск воркера

private:
	void Finish(); //Уведомление о завершении работы воркера
	QVariant CheckNullField(const QString &FieldName, const QVariantMap &VariantMap); //Проверка наличия поля

private:
	bool Auth(ISTcpMessage *TcpMessage);

private:
	QString DBConnectionName;
	QString DBHost;
	int DBPort;
	QString DBName;
	QString DBUser;
	QString DBPassword;
	QString ErrorString; //Текстовое описание ошибки запроса
	bool IsRunning; //Флаг занятости воркера
	ISTcpMessage *CurrentMessage; //Указатель на текущее сообщение

	//Критическая секция для синхронизации
#ifdef WIN32
	CRITICAL_SECTION CriticalSection;
#else
	pthread_mutex_t CriticalSection;
#endif
};
//-----------------------------------------------------------------------------
#endif
