#pragma once
//-----------------------------------------------------------------------------
#include "libcore_global.h"
#include "PMetaClassForeign.h"
//-----------------------------------------------------------------------------
class LIBCORE_EXPORT ISQueryModelHelper
{
public:
	static QString GetForeignViewNameField(const QString &MetaTableForeignAlias, PMetaClassForeign *MetaForeign, int Iterator); //Получить текст запроса для отображаемого поля эскортной таблицы (Атрибут в схеме ForeignViewNameField)
	static QString GetAliasForLeftJoinTable(const QString &TableAlias, int Iterator); //Получить алиас для таблицы в связке LEFT JOIN
};
//-----------------------------------------------------------------------------
