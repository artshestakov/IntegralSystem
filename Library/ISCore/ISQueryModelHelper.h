#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "PMetaClass.h"
//-----------------------------------------------------------------------------
class ISQueryModelHelper
{
public:
	static QString GetForeignViewNameField(const QString &MetaTableForeignAlias, PMetaForeign *MetaForeign, size_t Iterator); //Получить текст запроса для отображаемого поля эскортной таблицы (Атрибут в схеме ForeignViewNameField)
	static QString GetAliasForLeftJoinTable(const QString &TableAlias, size_t Iterator); //Получить алиас для таблицы в связке LEFT JOIN
};
//-----------------------------------------------------------------------------
