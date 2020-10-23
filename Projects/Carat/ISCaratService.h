#pragma once
#ifndef _ISCARATSERVICE_H_INCLUDED
#define _ISCARATSERVICE_H_INCLUDED
//-----------------------------------------------------------------------------
#include "qtservice.h"
//-----------------------------------------------------------------------------
class ISCaratService : public QtService<QCoreApplication>
{
public:
	ISCaratService(int argc, char **argv);
	virtual ~ISCaratService();

	void Stop()
	{
		stop();
	}

protected:
	void start() override; //Событие успешного запуска сервиса
	void stop() override; //Событие остановки сервиса
	void processCommand(int Code) override; //Событие входящей команды
	int executeApplication() override; //Событие инициализации сервиса

private:
	int Error(); //Выполняет остановку сервиса и возвращает ошибку
};
//-----------------------------------------------------------------------------
#endif
