#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "PMetaClass.h"
//-----------------------------------------------------------------------------
class ISDatabaseHelper
{
public:
	static QVariant CheckValue(QVariant &Value); //Проверка значения
	static QVariant GetObjectIDToList(PMetaClassTable *MetaTable, PMetaClassField *MetaField, int ObjectID); //Получить идентификатор объекта из справочника
	static int GetCountRows(const QString &TableName, const QString &Alias);
};
//-----------------------------------------------------------------------------
