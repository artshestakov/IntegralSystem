#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
//-----------------------------------------------------------------------------
class ISCaratService : public QObject
{
	Q_OBJECT

public:
	ISCaratService(QObject *parent = 0);
	virtual ~ISCaratService();

	void StartService(); //Запуск сервера

protected:
	void Started();
	void Finished(int ExitCode, QProcess::ExitStatus Status);
	void Error(QProcess::ProcessError ErrorType);
	void ReadyReadStandartOutput();

private:
	QEventLoop *EventLoop;
};
//-----------------------------------------------------------------------------
