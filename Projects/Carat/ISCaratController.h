#pragma once
#ifndef _ISCARATCONTROLLER_H_INCLUDED
#define _ISCARATCONTROLLER_H_INCLUDED
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISTypedefs.h"
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

	ISCriticalSection CriticalSection; //Критическая секция для синхронизации
};
//-----------------------------------------------------------------------------
#endif
