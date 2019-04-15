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
	
	void AddField(const QString &FieldName, const QString &Value); //Добавить параметр и его значение в ресурс
	QString GetParameterValue(const QString &ParameterName) const; //Получить значение параметра по его имени
	QStringMap GetParameters(); //Получить список всех параметров

	void SetTableName(const QString &table_name); //Изменить имя таблицы 
	QString GetTableName() const; //Получить имя таблицы

	void SetUID(const ISUuid &uid);
	ISUuid GetUID() const;

private:
	QString TableName;
	ISUuid UID;

	QStringMap Parameters;
};
//-----------------------------------------------------------------------------
