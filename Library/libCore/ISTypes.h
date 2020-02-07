#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISNamespace.h"
#include "ISUuid.h"
//-----------------------------------------------------------------------------
struct ISRangeStruct
{
public:
	ISRangeStruct()
	{

	}

	ISRangeStruct(const QVariant &begin_value, const QVariant &end_value)
		: BeginValue(begin_value),
		EndValue(end_value)
	{

	}

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
typedef QMap<QString, QString> QStringMap;
typedef QVector<int> QVectorInt;
typedef QVector<QString> QVectorString;
//-----------------------------------------------------------------------------
