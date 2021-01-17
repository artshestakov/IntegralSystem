#pragma once
#ifndef _ISCARATAPPLICATION_H_INCLUDED
#define _ISCARATAPPLICATION_H_INCLUDED
//-----------------------------------------------------------------------------
#include "ISCaratController.h"
#include "ISTcpServer.h"
#include "ISAsterisk.h"
//-----------------------------------------------------------------------------
class ISCaratApplication : public QCoreApplication
{
	Q_OBJECT

public:
    ISCaratApplication(int &argc, char **argv);
	virtual ~ISCaratApplication();

	QString GetErrorString() const; //Получить текстовое описание ошибки
	bool Initialize(); //Инициализация
    bool Run(const QStringList &Arguments); //Запуск с параметрами
	bool Run(); //Стандартный запуск
	void Shutdown(); //Остановка приложения

private:
    bool Help();
    bool Version();
    bool SendShutdown();
    bool ConfigCreate();
    bool ConfigReset();

private:
    bool SendCommand(const QByteArray &ByteArray);

private:
	QString ErrorString;
	ISCaratController *Controller;
	ISTcpServer *TcpServer;
	ISAsterisk *Asterisk;
};
//-----------------------------------------------------------------------------
#endif
