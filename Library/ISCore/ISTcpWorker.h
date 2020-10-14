#pragma once
#ifndef _ISTCPWORKER_H_INCLUDED
#define _ISTCPWORKER_H_INCLUDED
//-----------------------------------------------------------------------------
#include "iscore_global.h"
#include "ISStructs.h"
//-----------------------------------------------------------------------------
class ISCORE_EXPORT ISTcpWorker : public QObject
{
	Q_OBJECT

public:
	ISTcpWorker();
	virtual ~ISTcpWorker();

	bool GetRunning(); //Получить флаг занятости
	void SetMessage(ISTcpMessage *TcpMessage); //Установить сообщение на обработку
	void Run(); //Запуск воркера

private:
	void Finish(); //Уведомление о завершении работы воркера
	bool CheckNullField(const QString &FieldName, const QVariantMap &VariantMap); //Проверка наличия поля

private:
	bool Auth(ISTcpMessage *TcpMessage);

private:
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
