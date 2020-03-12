#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "PMetaClass.h"
//-----------------------------------------------------------------------------
class ISQueryModelHelper
{
public:
	static QString GetForeignViewNameField(const QString &MetaTableForeignAlias, PMetaForeign *MetaForeign, int Iterator); //Получить текст запроса для отображаемого поля эскортной таблицы (Атрибут в схеме ForeignViewNameField)
	static QString GetAliasForLeftJoinTable(const QString &TableAlias, int Iterator); //Получить алиас для таблицы в связке LEFT JOIN
};
//-----------------------------------------------------------------------------
