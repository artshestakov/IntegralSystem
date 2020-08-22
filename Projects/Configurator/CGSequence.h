#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
//-----------------------------------------------------------------------------
class CGSequence
{
public:
	static bool CreateSequence(const QString &TableName, QString &ErrorString); //—оздание последовательности
	static bool CheckExistSequence(const QString &TableName, bool &Exist, QString &ErrorString); //ѕроверка существовани€ последовательности
	static QString GetSequenceNameForTable(const QString &TableName); //ѕолучение имени последовательности
};
//-----------------------------------------------------------------------------
