#pragma once
//-----------------------------------------------------------------------------
#include "iscore_global.h"
#include "PMetaClass.h"
//-----------------------------------------------------------------------------
class ISCORE_EXPORT ISMetaDataHelper
{
public:
	static QString GenerateSqlQueryFromForeign(PMetaForeign *MetaForeign, const QString &SqlFilter = QString(), const QVariant &ObjectID = QVariant()); //Генерация SQL-запроса для внешнего ключа
	static QString GetConfigurationName(); //Получить наименование конфигурации БД
};
//-----------------------------------------------------------------------------
