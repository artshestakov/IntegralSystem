#pragma once
//-----------------------------------------------------------------------------
#include "libCore_global.h"
//-----------------------------------------------------------------------------
class LIBCORE_EXPORT ISCountingTime
{
public:
	ISCountingTime();
	~ISCountingTime();

	unsigned int Elapsed(); //�������� ��������� ������ � ������������

private:
	clock_t StartClock;
};
//-----------------------------------------------------------------------------
