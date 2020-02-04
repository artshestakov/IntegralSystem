#pragma once
#ifndef _PMETACLASSRESOURCE_H_INCLUDED
#define _PMETACLASSRESOURCE_H_INCLUDED
//-----------------------------------------------------------------------------
#include "libCore_global.h"
#include "ISUuid.h"
#include "ISTypes.h"
//-----------------------------------------------------------------------------
class LIBCORE_EXPORT PMetaClassResource : public QObject
{
	Q_OBJECT

public:
	PMetaClassResource(QObject *parent = 0);
	virtual ~PMetaClassResource();
	
	void AddField(const QString &FieldName, const QString &Value); //�������� �������� � ��� �������� � ������
	QString GetParameterValue(const QString &ParameterName) const; //�������� �������� ��������� �� ��� �����
	QStringMap GetParameters(); //�������� ������ ���� ����������

	void SetTableName(const QString &table_name); //�������� ��� ������� 
	QString GetTableName() const; //�������� ��� �������

	void SetUID(const ISUuid &uid);
	ISUuid GetUID() const;

private:
	QString TableName;
	ISUuid UID;

	QStringMap Parameters;
};
//-----------------------------------------------------------------------------
#endif
