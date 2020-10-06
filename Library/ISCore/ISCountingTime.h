#pragma once
//-----------------------------------------------------------------------------
#include "iscore_global.h"
//-----------------------------------------------------------------------------
class ISCORE_EXPORT ISCountingTime
{
public:
	ISCountingTime();
	~ISCountingTime();

    unsigned long long Elapsed(); //�������� ��������� ������ � ������������

private:
    std::chrono::steady_clock::time_point StartClock;
};
//-----------------------------------------------------------------------------
