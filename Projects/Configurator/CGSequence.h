#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
//-----------------------------------------------------------------------------
class CGSequence
{
public:
	static bool CreateSequence(const QString &ClassName); //Создание последовательности
	static bool CheckExistSequence(const QString &ClassName, bool &Exist, QString &ErrorString); //Проверка существования последовательности
	static QString GetSequenceNameForClass(const QString &ClassName); //Получение имени последовательности
};
//-----------------------------------------------------------------------------
