#pragma once
//-----------------------------------------------------------------------------
#include "libcore_global.h"
#include "PMetaClassTable.h"
#include "PMetaClassField.h"
//-----------------------------------------------------------------------------
class LIBCORE_EXPORT ISDatabaseHelper : public QObject
{
	Q_OBJECT

public:
	ISDatabaseHelper();
	virtual ~ISDatabaseHelper();

	static QVariant CheckValue(QVariant &Value); //Проверка значения
	static QVariant GetObjectIDToList(PMetaClassTable *MetaTable, PMetaClassField *MetaField, int ObjectID); //Получить идентификатор объекта из справочника
	static int GetCountRows(const QString &TableName, const QString &Alias);
};
//-----------------------------------------------------------------------------
