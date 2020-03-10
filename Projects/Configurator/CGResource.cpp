#include "CGResource.h"
#include "ISQuery.h"
#include "ISMetaData.h"
#include "ISAssert.h"
#include "ISSystem.h"
#include "ISAlgorithm.h"
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

	int CountParameters = MetaResource->Parameters.size();
	for (int i = 0; i < CountParameters; ++i)
	{
		QString FieldName = TableAlias + '_' + ConvertMapToKeys(MetaResource->Parameters)[i].toLower();
		InsertText += FieldName + ", ";
		ValuesText += ':' + FieldName + ", ";
	}

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
		QString FieldName = TableAlias + '_' + ConvertMapToKeys(MetaResource->Parameters)[i].toLower();
		QString FieldValue = ConvertMapToValues(MetaResource->Parameters)[i];
		bool BindValue = qInsertResource.BindValue(':' + FieldName, FieldValue);
		IS_ASSERT(BindValue, QString("Not BindValue. TableName: %1. FieldName: %2").arg(MetaResource->TableName).arg(FieldName));
	}

	bool Result = qInsertResource.Execute();
	if (!Result)
	{
		ErrorString = qInsertResource.GetErrorString();
	}
	return Result;
}
//-----------------------------------------------------------------------------
bool CGResource::UpdateResource(PMetaClassResource *MetaResource, QString &ErrorString)
{
	QString TableName = MetaResource->TableName;
	QString TableAlias = ISMetaData::GetInstanse().GetMetaTable(TableName)->Alias;
	QString ResourceUID = MetaResource->UID;

    bool Result = UpdateResourceField(TableName, TableAlias, TableAlias + "_isdeleted", false, ResourceUID, ErrorString);
	IS_ASSERT(Result, QString("Error update resource field. TableName: %1 FieldName: %2 UID: %3 Error: %4").arg(TableName).arg(TableAlias + "_isdeleted").arg(ResourceUID).arg(ErrorString));

    Result = UpdateResourceField(TableName, TableAlias, TableAlias + "_issystem", false, ResourceUID, ErrorString);
	IS_ASSERT(Result, QString("Error update resource field. TableName: %1 FieldName: %2 UID: %3 Error: %4").arg(TableName).arg(TableAlias + "_issystem").arg(ResourceUID).arg(ErrorString));

	for (const auto &Resource : MetaResource->Parameters) //ќбход параметров ресурса
	{
		QString FieldName = Resource.first;
		QString FieldNameComplete = QString(TableAlias + '_' + Resource.first).toLower();
		QString FieldValue = Resource.second;

		PMetaClassField *MetaField = ISMetaData::GetInstanse().GetMetaField(TableName, FieldName);
		if (!MetaField->NotNull) //≈сли поле Ќ≈ я¬Ћя≈“—я об€зательным дл€ заполнени€ - обнулить его
		{
			bool ResetField = ResetResourceField(TableName, TableAlias, FieldNameComplete, ResourceUID, ErrorString);
			IS_ASSERT(ResetField, QString("Not reset resource field. TableName: %1. FieldName: %2. UID: %3. Error: %4").arg(TableName).arg(FieldNameComplete).arg(ResourceUID).arg(ErrorString));
		}

		Result = UpdateResourceField(TableName, TableAlias, FieldNameComplete, FieldValue, ResourceUID, ErrorString);
		IS_ASSERT(Result, QString("Not update resource. TableName: %1. FieldName: %2. UID: %3. Error: %4").arg(TableName).arg(FieldNameComplete).arg(ResourceUID).arg(ErrorString));
	}

	PMetaClassTable *MetaTable = ISMetaData::GetInstanse().GetMetaTable(TableName);
	for (int i = 0; i < MetaTable->Fields.size(); ++i) //ќбход пользовательских полей таблицы и их очистка
	{
		PMetaClassField *MetaField = MetaTable->Fields[i];
		if (MetaField->NotNull || MetaField->QueryText.length())
		{
			continue;
		}

		if (!VectorContains(ConvertMapToKeys(MetaResource->Parameters), MetaField->Name))
		{
			bool ResetUserField = ResetResourceField(TableName, TableAlias, TableAlias + '_' + MetaField->Name, ResourceUID, ErrorString);
			IS_ASSERT(ResetUserField, QString("Not reset resource field. TableName: %1. FieldName: %2. UID: %3. Error: %4").arg(TableName).arg(TableAlias + '_' + MetaField->Name).arg(ResourceUID).arg(ErrorString));
		}
	}
	return Result;
}
//-----------------------------------------------------------------------------
bool CGResource::CheckExistResource(PMetaClassResource *MetaResource, bool &Exist, QString &ErrorString)
{
	ISQuery qSelectResource(QS_RESOURCE.arg(MetaResource->TableName).arg(ISMetaData::GetInstanse().GetMetaTable(MetaResource->TableName)->Alias));
	qSelectResource.SetShowLongQuery(false);
	qSelectResource.BindValue(":ResourceUID", MetaResource->UID);
	bool Result = qSelectResource.ExecuteFirst();
	if (Result)
	{
		Exist = qSelectResource.ReadColumn("count").toInt() > 0;
	}
	else
	{
		ErrorString = qSelectResource.GetErrorString();
	}
	return Result;
}
//-----------------------------------------------------------------------------
bool CGResource::ResetResourceField(const QString &TableName, const QString &TableAlias, const QString &FieldName, const QString &ResourceUID, QString &ErrorString)
{
	ISQuery qReset(QU_RESET_RESOURCE_FIELD.arg(TableName).arg(FieldName).arg(TableAlias));
	qReset.SetShowLongQuery(false);
	qReset.BindValue(":ResourceUID", ResourceUID);
	bool Result = qReset.Execute();
	if (!Result)
	{
		ErrorString = qReset.GetErrorString();
	}
	return Result;
}
//-----------------------------------------------------------------------------
bool CGResource::UpdateResourceField(const QString &TableName, const QString &TableAlias, const QString &FieldName, const QVariant &Value, const QString &ResourceUID, QString &ErrorString)
{
	ISQuery qUpdate(QU_RESOURCE_FIELD.arg(TableName).arg(FieldName).arg(TableAlias));
	qUpdate.SetShowLongQuery(false);
	qUpdate.BindValue(":ResourceValue", Value);
	qUpdate.BindValue(":ResourceUID", ResourceUID);
	bool Result = qUpdate.Execute();
	if (!Result)
	{
		ErrorString = qUpdate.GetErrorString();
	}
	return Result;
}
//-----------------------------------------------------------------------------
