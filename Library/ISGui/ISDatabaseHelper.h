#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "PMetaClass.h"
//-----------------------------------------------------------------------------
class ISDatabaseHelper
{
public:
	static void CheckValue(QVariant &Value); //Проверка значения
	static QVariant GetObjectIDToList(PMetaTable *MetaTable, PMetaField *MetaField, int ObjectID); //Получить идентификатор объекта из справочника
	static int GetCountRows(const QString &TableName);
};
//-----------------------------------------------------------------------------
