#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
//-----------------------------------------------------------------------------
class ISCountingTime
{
public:
	ISCountingTime(QObject *parent = 0);
	virtual ~ISCountingTime();

	int GetElapsed(); //Получить результат замера в милисекундах
	int GetElapsedRestart(); //Получить результат замера в милисекундах и сбросить таймер
	void Restart(); //Обнуление счётчика времени

private:
	QTime Time;
	int Elapsed;
};
//-----------------------------------------------------------------------------
