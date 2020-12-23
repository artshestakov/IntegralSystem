#pragma once
//-----------------------------------------------------------------------------
#include "iscore_global.h"
#include "PMetaClass.h"
//-----------------------------------------------------------------------------
class ISCORE_EXPORT ISCore
{
public:
	static QString GetObjectName(const QString &TableName, int ObjectID); //Получить наименование объекта
	static QString GetObjectName(PMetaTable *MetaTable, int ObjectID); //Получить наименование объекта

	//Номера телефонов
	static void PhoneNumberPrepare(QString &PhoneNumber); //Подготовить номер телефона (удалить все кроме цифр)
	static void PhoneNumberFormat(QString &PhoneNumber); //Форматировать номер телефона
};
//-----------------------------------------------------------------------------
