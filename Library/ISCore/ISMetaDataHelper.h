#pragma once
//-----------------------------------------------------------------------------
#include "iscore_global.h"
#include "ISNamespace.h"
#include "PMetaClassForeign.h"
//-----------------------------------------------------------------------------
class ISCORE_EXPORT ISMetaDataHelper : public QObject
{
	Q_OBJECT

public:
	ISMetaDataHelper();
	virtual ~ISMetaDataHelper();

	static QString GenerateSqlQueryFromForeign(PMetaClassForeign *MetaForeign, const QString &SqlFilter = QString(), const QVariant &ObjectID = QVariant()); //Генерация SQL-запроса для внешнего ключа
};
//-----------------------------------------------------------------------------
