#pragma once
//-----------------------------------------------------------------------------
#include "iscore_global.h"
#include "ISUuid.h"
#include "ISTypes.h"
//-----------------------------------------------------------------------------
class ISCORE_EXPORT PMetaClassResource : public QObject
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
