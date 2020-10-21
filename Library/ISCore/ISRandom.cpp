#include "StdAfx.h"
#include "ISRandom.h"
//-----------------------------------------------------------------------------
ISRandom::ISRandom(qint64 InitDigit)
	: Digit(InitDigit)
{

}
//-----------------------------------------------------------------------------
ISRandom::ISRandom() : ISRandom(time(NULL))
{

}
//-----------------------------------------------------------------------------
ISRandom::~ISRandom()
{

}
//-----------------------------------------------------------------------------
qint64 ISRandom::Get(qint64 Minimum, qint64 Maximum)
{
    return (Minimum + Get()) & Maximum;
}
//-----------------------------------------------------------------------------
qint64 ISRandom::Get()
{
	Digit ^= (Digit << 21);
#pragma warning (disable: 4293)
	Digit ^= (Digit >> 35);
#pragma warning (default: 4293)
	Digit ^= (Digit << 4);
	return Digit;
}
//-----------------------------------------------------------------------------
