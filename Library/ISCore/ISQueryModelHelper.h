#pragma once
//-----------------------------------------------------------------------------
#include "iscore_global.h"
#include "PMetaClassForeign.h"
//-----------------------------------------------------------------------------
class ISCORE_EXPORT ISQueryModelHelper : public QObject
{
	Q_OBJECT

public:
	ISQueryModelHelper();
	virtual ~ISQueryModelHelper();

	static QString GetForeignViewNameField(const QString &MetaTableForeignAlias, PMetaClassForeign *MetaForeign, int Iterator); //Получить текст запроса для отображаемого поля эскортной таблицы (Атрибут в схеме ForeignViewNameField)
	static QString GetAliasForLeftJoinTable(const QString &TableAlias, int Iterator); //Получить алиас для таблицы в связке LEFT JOIN
};
//-----------------------------------------------------------------------------
