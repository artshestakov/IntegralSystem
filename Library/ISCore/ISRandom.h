#pragma once
#ifndef _ISRANDOM_H_INCLUDED
#define _ISRANDOM_H_INCLUDED
//-----------------------------------------------------------------------------
#include "iscore_global.h"
//-----------------------------------------------------------------------------
class ISCORE_EXPORT ISRandom
{
public:
	ISRandom(qint64 InitDigit);
	ISRandom();
	~ISRandom();

	qint64 Get(qint64 Minimum, qint64 Maximum);
	qint64 Get();

private:
	qint64 Digit;
};
//-----------------------------------------------------------------------------
#endif
