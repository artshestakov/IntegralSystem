#pragma once
#ifndef _ISRANDOM_H_INCLUDED
#define _ISRANDOM_H_INCLUDED
//-----------------------------------------------------------------------------
#include "iscore_global.h"
//-----------------------------------------------------------------------------
typedef unsigned __int64 rand_t;
//-----------------------------------------------------------------------------
class ISCORE_EXPORT ISRandom
{
public:
	ISRandom(rand_t InitDigit);
	ISRandom();
	~ISRandom();

	rand_t Get(rand_t Minimum, rand_t Maximum);
	rand_t Get();

private:
	rand_t Digit;
};
//-----------------------------------------------------------------------------
#endif
