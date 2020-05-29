#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
//-----------------------------------------------------------------------------
class ISServiceController
{
public:
	ISServiceController(const QString &service_name);
	~ISServiceController();

	QString GetErrorString() const; //Получить описание последней ошибки
	bool Start(); //Запуск службы
	bool Stop(); //Остановка службы
	bool IsExist(); //Наличие службы
	bool IsRunning(bool &IsRunning); //Проверка активности службы

private:
	bool OpenDB(unsigned long Access);
	void CloseDB();

private:
	QString ErrorString;
	char *ServiceName;

	SC_HANDLE SCManager;
	SC_HANDLE SCService;
};
//-----------------------------------------------------------------------------
