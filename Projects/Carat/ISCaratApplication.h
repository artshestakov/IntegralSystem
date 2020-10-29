#pragma once
#ifndef _ISCARATAPPLICATION_H_INCLUDED
#define _ISCARATAPPLICATION_H_INCLUDED
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISCaratController.h"
#include "ISTcpServer.h"
//-----------------------------------------------------------------------------
class ISCaratApplication : public QCoreApplication
{
	Q_OBJECT

signals:
	void Quit();

public:
    ISCaratApplication(int &argc, char **argv);
	virtual ~ISCaratApplication();

	QString GetErrorString() const; //Получить текстовое описание ошибки
	bool Init(); //Инициализация
	void Run(const QStringList &Arguments); //Запуск с параметрами
	bool Run(); //Стандартный запуск

private:
	void Shutdown();
	void Help();
	void Version();
	void SendShutdown();
	void SendCommand(const QByteArray &ByteArray);

private:
	QString ErrorString;
	ISCaratController *Controller;
	ISTcpServer *TcpServer;
};
//-----------------------------------------------------------------------------
#endif
