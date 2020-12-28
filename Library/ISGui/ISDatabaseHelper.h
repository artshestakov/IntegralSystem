#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "PMetaClass.h"
//-----------------------------------------------------------------------------
class ISDatabaseHelper
{
public:
	static QVariant GetObjectIDToList(PMetaTable *MetaTable, PMetaField *MetaField, int ObjectID); //Получить идентификатор объекта из справочника
};
//-----------------------------------------------------------------------------
