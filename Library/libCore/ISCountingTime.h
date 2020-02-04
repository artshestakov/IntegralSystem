#pragma once
//-----------------------------------------------------------------------------
#include "libCore_global.h"
//-----------------------------------------------------------------------------
class LIBCORE_EXPORT ISCountingTime
{
public:
	ISCountingTime();
	~ISCountingTime();

	int GetElapsed(); //Получить результат замера в милисекундах
	int GetElapsedRestart(); //Получить результат замера в милисекундах и сбросить таймер
	void Restart(); //Обнуление счётчика времени

private:
	QTime Time;
	int Elapsed;
};
//-----------------------------------------------------------------------------
