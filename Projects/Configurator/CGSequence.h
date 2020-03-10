#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
//-----------------------------------------------------------------------------
class CGSequence
{
public:
	static bool CreateSequence(const QString &ClassName); //—оздание последовательности
	static bool CheckExistSequence(const QString &ClassName, bool &Exist, QString &ErrorString); //ѕроверка существовани€ последовательности
	static QString GetSequenceNameForClass(const QString &ClassName); //ѕолучение имени последовательности
};
//-----------------------------------------------------------------------------
