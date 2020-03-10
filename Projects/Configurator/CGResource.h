#pragma once
//-----------------------------------------------------------------------------
#include "PMetaClass.h"
//-----------------------------------------------------------------------------
class CGResource
{
public:
	static bool InsertResource(PMetaClassResource *MetaResource, QString &ErrorString);
	static bool UpdateResource(PMetaClassResource *MetaResource, QString &ErrorString);
	static bool CheckExistResource(PMetaClassResource *MetaResource, bool &Exist, QString &ErrorString);

protected:
    static bool ResetResourceField(const QString &TableName, const QString &TableAlias, const QString &FieldName, const QString &ResourceUID, QString &ErrorString); //�������� ���� ������� � �������
    static bool UpdateResourceField(const QString &TableName, const QString &TableAlias, const QString &FieldName, const QVariant &Value, const QString &ResourceUID, QString &ErrorString); //�������� ���� ������� � �������
};
//-----------------------------------------------------------------------------
