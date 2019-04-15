#pragma once
//-----------------------------------------------------------------------------
#include "isconfigurator_global.h"
#include "PMetaClassResource.h"
//-----------------------------------------------------------------------------
class ISCONFIGURATOR_EXPORT CGResource : public QObject
{
	Q_OBJECT

public:
	CGResource();
	virtual ~CGResource();

	static bool InsertResource(PMetaClassResource *MetaResource, QString &ErrorString);
	static void UpdateResource(PMetaClassResource *MetaResource);
	static bool CheckExistResource(PMetaClassResource *MetaResource);

protected:
    static bool ResetResourceField(const QString &TableName, const QString &TableAlias, const QString &FieldName, const QString &ResourceUID, QString &ErrorString); //�������� ���� ������� � �������
    static bool UpdateResourceField(const QString &TableName, const QString &TableAlias, const QString &FieldName, const QVariant &Value, const QString &ResourceUID, QString &ErrorString); //�������� ���� ������� � �������
};
//-----------------------------------------------------------------------------
