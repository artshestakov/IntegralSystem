#pragma once
#ifndef _ISSTRUCTS_H_INCLUDED
#define _ISSTRUCTS_H_INCLUDED
//-----------------------------------------------------------------------------
struct ISDateTime
{
	ISDateTime()
		: Day(0), Month(0), Year(0), Hour(0), Minute(0), Second(0), Milliseconds(0) { }

	ISDateTime(unsigned short day, unsigned short month, unsigned short year, unsigned short hour, unsigned short minute, unsigned short second, unsigned short milliseconds)
		: Day(day), Month(month), Year(year), Hour(hour), Minute(minute), Second(second), Milliseconds(milliseconds) { }

	unsigned short Day;
	unsigned short Month;
	unsigned short Year;
	unsigned short Hour;
	unsigned short Minute;
	unsigned short Second;
	unsigned short Milliseconds;
};
//-----------------------------------------------------------------------------
#endif
