#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
//-----------------------------------------------------------------------------
class CGSequence
{
public:
	static bool CreateSequence(const QString &TableName, QString &ErrorString); //Создание последовательности
	static bool CheckExistSequence(const QString &TableName, bool &Exist, QString &ErrorString); //Проверка существования последовательности
	static QString GetSequenceNameForTable(const QString &TableName); //Получение имени последовательности
};
//-----------------------------------------------------------------------------
