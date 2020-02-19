#include "CGResource.h"
#include "ISQuery.h"
#include "ISMetaData.h"
#include "ISAssert.h"
#include "ISSystem.h"
//-----------------------------------------------------------------------------
static QString QS_RESOURCE = "SELECT COUNT(*) FROM %1 WHERE %2_uid = :ResourceUID";
//-----------------------------------------------------------------------------
static QString QU_RESET_RESOURCE_FIELD = "UPDATE %1 SET %2 = NULL WHERE %3_uid = :ResourceUID";
//-----------------------------------------------------------------------------
static QString QU_RESOURCE_FIELD = "UPDATE %1 SET %2 = :ResourceValue WHERE %3_uid = :ResourceUID";
//-----------------------------------------------------------------------------
bool CGResource::InsertResource(PMetaClassResource *MetaResource, QString &ErrorString)
{
	QString TableAlias = ISMetaData::GetInstanse().GetMetaTable(MetaResource->TableName)->Alias;

	QString InsertText = "INSERT INTO " + MetaResource->TableName.toLower() + '(' + TableAlias + "_uid, ";
	QString ValuesText = "VALUES(:UID, ";

	int CountParameters = MetaResource->Parameters.count(); //Количество параметров
	for (int i = 0; i < CountParameters; ++i) //Обход параметров ресурса
	{
		QString FieldName = TableAlias + '_' + MetaResource->Parameters.keys().at(i).toLower();

		InsertText += FieldName + ", ";
		ValuesText += ':' + FieldName + ", ";
	}

	//Подготовка запроса на добавление ресурса
	ISSystem::RemoveLastSymbolFromString(InsertText, 2);
	InsertText += ") \n";

	ISSystem::RemoveLastSymbolFromString(ValuesText, 2);
	ValuesText += ");";

	ISQuery qInsertResource(InsertText + ValuesText);
	qInsertResource.SetShowLongQuery(false);

	bool BindValueUID = qInsertResource.BindValue(":UID", MetaResource->UID);
	IS_ASSERT(BindValueUID, QString("Not BindValue Field UID. UID: %1").arg(MetaResource->UID));

	for (int i = 0; i < CountParameters; ++i)
	{
		QString FieldName = TableAlias + '_' + MetaResource->Parameters.keys().at(i).toLower();
		QString FieldValue = MetaResource->Parameters.values().at(i);
		bool BindValue = qInsertResource.BindValue(':' + FieldName, FieldValue);
		IS_ASSERT(BindValue, QString("Not BindValue. TableName: %1. FieldName: %2").arg(MetaResource->TableName).arg(FieldName));
	}

	bool Inserting = qInsertResource.Execute();
	if (!Inserting)
	{
		ErrorString = qInsertResource.GetErrorText();
	}

	return Inserting;
}
//-----------------------------------------------------------------------------
void CGResource::UpdateResource(PMetaClassResource *MetaResource)
{
	QString TableName = MetaResource->TableName;
	QString TableAlias = ISMetaData::GetInstanse().GetMetaTable(TableName)->Alias;
	QString ResourceUID = MetaResource->UID;

    QString ErrorString;
    UpdateResourceField(TableName, TableAlias, TableAlias + "_isdeleted", false, ResourceUID, ErrorString);
    UpdateResourceField(TableName, TableAlias, TableAlias + "_issystem", false, ResourceUID, ErrorString);

	QStringMap Parameters = MetaResource->Parameters;
	for (const auto &Resource : Parameters.toStdMap()) //Обход параметров ресурса
	{
		QString FieldName = Resource.first;
		QString FieldNameComplete = QString(TableAlias + '_' + Resource.first).toLower();
		QString FieldValue = Resource.second;

		QString ErrorString;
		PMetaClassField *MetaField = ISMetaData::GetInstanse().GetMetaField(TableName, FieldName);
		if (!MetaField->NotNull) //Если поле НЕ ЯВЛЯЕТСЯ обязательным для заполнения - обнулить его
		{
			bool ResetField = ResetResourceField(TableName, TableAlias, FieldNameComplete, ResourceUID, ErrorString);
			IS_ASSERT(ResetField, QString("Not reset resource field. TableName: %1. FieldName: %2. UID: %3. Error: %4").arg(TableName).arg(FieldNameComplete).arg(ResourceUID).arg(ErrorString));
		}

		//Обновление поля ресурса
		bool Updated = UpdateResourceField(TableName, TableAlias, FieldNameComplete, FieldValue, ResourceUID, ErrorString);
		IS_ASSERT(Updated, QString("Not update resource. TableName: %1. FieldName: %2. UID: %3. Error: %4").arg(TableName).arg(FieldNameComplete).arg(ResourceUID).arg(ErrorString));
	}

	PMetaClassTable *MetaTable = ISMetaData::GetInstanse().GetMetaTable(TableName);
	for (int i = 0; i < MetaTable->Fields.count(); ++i) //Обход пользовательских полей таблицы и их очистка
	{
		PMetaClassField *MetaField = MetaTable->Fields[i];

		if (MetaField->NotNull || MetaField->QueryText.length())
		{
			continue;
		}

		if (!Parameters.keys().contains(MetaField->Name))
		{
			QString ErrorString;
			bool ResetUserField = ResetResourceField(TableName, TableAlias, TableAlias + '_' + MetaField->Name, ResourceUID, ErrorString);
			IS_ASSERT(ResetUserField, QString("Not reset resource field. TableName: %1. FieldName: %2. UID: %3. Error: %4").arg(TableName).arg(TableAlias + '_' + MetaField->Name).arg(ResourceUID).arg(ErrorString));
		}
	}

}
//-----------------------------------------------------------------------------
bool CGResource::CheckExistResource(PMetaClassResource *MetaResource)
{
	QString SqlText = QS_RESOURCE.arg(MetaResource->TableName).arg(ISMetaData::GetInstanse().GetMetaTable(MetaResource->TableName)->Alias);
	ISUuid UID = MetaResource->UID;

	ISQuery qSelectResource(SqlText);
	qSelectResource.SetShowLongQuery(false);
	qSelectResource.BindValue(":ResourceUID", UID);
	if (qSelectResource.ExecuteFirst())
	{
		int Count = qSelectResource.ReadColumn("count").toInt();
		if (Count)
		{
			return true;
		}
	}

	return false;
}
//-----------------------------------------------------------------------------
bool CGResource::ResetResourceField(const QString &TableName, const QString &TableAlias, const QString &FieldName, const QString &ResourceUID, QString &ErrorString)
{
	QString QueryText = QU_RESET_RESOURCE_FIELD.arg(TableName).arg(FieldName).arg(TableAlias);

	ISQuery qReset(QueryText);
	qReset.SetShowLongQuery(false);
	qReset.BindValue(":ResourceUID", ResourceUID);
	bool Reset = qReset.Execute();
	if (!Reset)
	{
		ErrorString = qReset.GetErrorText();
	}

	return Reset;
}
//-----------------------------------------------------------------------------
bool CGResource::UpdateResourceField(const QString &TableName, const QString &TableAlias, const QString &FieldName, const QVariant &Value, const QString &ResourceUID, QString &ErrorString)
{
	QString QueryText = QU_RESOURCE_FIELD.arg(TableName).arg(FieldName).arg(TableAlias);

	ISQuery qUpdate(QueryText);
	qUpdate.SetShowLongQuery(false);
	qUpdate.BindValue(":ResourceValue", Value);
	qUpdate.BindValue(":ResourceUID", ResourceUID);
	bool Updated = qUpdate.Execute();
	if (!Updated)
	{
		ErrorString = qUpdate.GetErrorText();
	}

	return Updated;
}
//-----------------------------------------------------------------------------
