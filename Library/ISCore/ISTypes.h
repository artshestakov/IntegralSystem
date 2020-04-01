#pragma once
#ifndef _ISTYPES_H_INCLUDED
#define _ISTYPES_H_INCLUDED
//-----------------------------------------------------------------------------
#include "StdAfx.h"
//-----------------------------------------------------------------------------
struct ISRangeStruct
{
	ISRangeStruct() { }
	ISRangeStruct(const QVariant &begin_value, const QVariant &end_value) : BeginValue(begin_value), EndValue(end_value) { }

	void Clear()
	{
		BeginValue.clear();
		EndValue.clear();
	}

	bool IsValid() const
	{
		return BeginValue.isValid() && EndValue.isValid();
	}

	QVariant BeginValue;
	QVariant EndValue;
};
//-----------------------------------------------------------------------------
#endif
