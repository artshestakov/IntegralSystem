#pragma once
//-----------------------------------------------------------------------------
#include "iscore_global.h"
//-----------------------------------------------------------------------------
class ISCORE_EXPORT ISCountingTime
{
public:
	ISCountingTime();
	~ISCountingTime();

    unsigned long long Elapsed(); //Получить результат замера в милисекундах

private:
    std::chrono::steady_clock::time_point StartClock;
};
//-----------------------------------------------------------------------------
