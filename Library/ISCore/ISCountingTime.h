#pragma once
//-----------------------------------------------------------------------------
#include "iscore_global.h"
//-----------------------------------------------------------------------------
class ISCORE_EXPORT ISCountingTime
{
public:
	ISCountingTime();
	~ISCountingTime();

	clock_t Elapsed(); //�������� ��������� ������ � ������������

private:
	clock_t StartClock;
};
//-----------------------------------------------------------------------------
