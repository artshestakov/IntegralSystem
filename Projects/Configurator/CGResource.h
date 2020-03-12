#pragma once
//-----------------------------------------------------------------------------
#include "PMetaClass.h"
//-----------------------------------------------------------------------------
class CGResource
{
public:
	static bool InsertResource(PMetaResource *MetaResource, QString &ErrorString);
	static bool UpdateResource(PMetaResource *MetaResource, QString &ErrorString);
	static bool CheckExistResource(PMetaResource *MetaResource, bool &Exist, QString &ErrorString);

protected:
    static bool ResetResourceField(const QString &TableName, const QString &TableAlias, const QString &FieldName, const QString &ResourceUID, QString &ErrorString); //Обнулить поле ресурса в таблице
    static bool UpdateResourceField(const QString &TableName, const QString &TableAlias, const QString &FieldName, const QVariant &Value, const QString &ResourceUID, QString &ErrorString); //Обновить поле ресурса в таблице
};
//-----------------------------------------------------------------------------
