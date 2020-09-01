#include "StdAfx.h"
#include "ISRandom.h"
//-----------------------------------------------------------------------------
ISRandom::ISRandom(rand_t InitDigit)
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
rand_t ISRandom::Get(rand_t Minimum, rand_t Maximum)
{
	return Minimum + Get() & Maximum;
}
//-----------------------------------------------------------------------------
rand_t ISRandom::Get()
{
	Digit ^= (Digit << 21);
#pragma warning (disable: 4293)
	Digit ^= (Digit >> 35);
#pragma warning (default: 4293)
	Digit ^= (Digit << 4);
	return Digit;
}
//-----------------------------------------------------------------------------
