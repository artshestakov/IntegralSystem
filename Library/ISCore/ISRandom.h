#pragma once
#ifndef _ISRANDOM_H_INCLUDED
#define _ISRANDOM_H_INCLUDED
//-----------------------------------------------------------------------------
#include "iscore_global.h"
//-----------------------------------------------------------------------------
#ifdef WIN32
typedef __int64 rand_t;
#else
typedef int64_t rand_t;
#endif
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
