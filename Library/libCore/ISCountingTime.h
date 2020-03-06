#pragma once
//-----------------------------------------------------------------------------
#include "libCore_global.h"
//-----------------------------------------------------------------------------
class LIBCORE_EXPORT ISCountingTime
{
public:
	ISCountingTime();
	~ISCountingTime();

	clock_t Elapsed(); //Получить результат замера в милисекундах

private:
	clock_t StartClock;
};
//-----------------------------------------------------------------------------
