#pragma once
//-----------------------------------------------------------------------------
#include "iscore_global.h"
#include "PMetaClass.h"
//-----------------------------------------------------------------------------
class ISCORE_EXPORT ISMetaDataHelper
{
public:
	static QString GenerateSqlQueryFromTitleName(PMetaForeign *MetaForeign, const QString &Alias, const QString &FieldName); //Генерация SQL-запроса для получения имени эскортной ссылки
};
//-----------------------------------------------------------------------------
