#pragma once
#ifndef _ISCARATCONTROLLER_H_INCLUDED
#define _ISCARATCONTROLLER_H_INCLUDED
//-----------------------------------------------------------------------------
#include "StdAfx.h"
//-----------------------------------------------------------------------------
class ISCaratController : public QThread
{
	Q_OBJECT

signals:
	void Started();
	void Shutdown();

public:
	ISCaratController(QObject *parent);
	virtual ~ISCaratController();

	QString GetErrorString() const; //Получить текстовое описание ошибки
	bool Start(); //Запуск потока
	void Stop(); //Остановка потока

protected:
	void run() override;

private:
	void Send(QTcpSocket *TcpSocket, const QByteArray &Responce);
	void Wait();

private:
	QString ErrorString;
	unsigned short ControllerPort;
	bool IsRunning;
	bool IsFinished;

	//Критическая секция для синхронизации
#ifdef WIN32
	CRITICAL_SECTION CriticalSection;
#else
	pthread_mutex_t CriticalSection;
#endif
};
//-----------------------------------------------------------------------------
#endif
