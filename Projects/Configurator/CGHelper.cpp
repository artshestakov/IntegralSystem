#include "CGHelper.h"
#include "ISQuery.h"
#include "ISSystem.h"
#include "ISMetaData.h"
//-----------------------------------------------------------------------------
static QString QS_COLUMN = PREPARE_QUERY("SELECT COUNT(*) "
										 "FROM information_schema.columns "
										 "WHERE table_catalog = current_database() "
										 "AND table_schema = current_schema() "
										 "AND table_name = :TableName "
										 "AND column_name = :ColumnName");
//-----------------------------------------------------------------------------
bool CGHelper::CheckExistColumn(PMetaTable *MetaTable, const QString &ColumnName, bool &Exist, QString &ErrorString)
{
	ISQuery qSelectColumn(QS_COLUMN);
	qSelectColumn.SetShowLongQuery(false);
	qSelectColumn.BindValue(":TableName", MetaTable->Name.toLower());
	qSelectColumn.BindValue(":ColumnName", ColumnName);
	bool Result = qSelectColumn.ExecuteFirst();
	if (Result)
	{
		Exist = qSelectColumn.ReadColumn("count").toInt() > 0;
	}
	else
	{
		ErrorString = qSelectColumn.GetErrorString();
	}
	return Result;
}
//-----------------------------------------------------------------------------
bool CGHelper::CommentTable(PMetaTable *MetaTable, QString &ErrorString)
{
	QString CommentText = ISSystem::VariantMapToJsonString(
	{
		{ "Name", MetaTable->Name },
		{ "UID", MetaTable->UID },
		{ "Alias", MetaTable->Alias },
		{ "LocalName", MetaTable->LocalName },
		{ "LocalListName", MetaTable->LocalListName },
		{ "TitleName", MetaTable->TitleName },
		{ "ObjectForm", MetaTable->ObjectForm },
		{ "ShowOnly", MetaTable->ShowOnly },
		{ "IsSystem", MetaTable->IsSystem }
	});

	ISQuery qComment;
	qComment.SetShowLongQuery(false);
	bool Result = qComment.Execute(QString("COMMENT ON TABLE public.%1 IS '%2'").arg(MetaTable->Name.toLower()).arg(CommentText));
	if (!Result)
	{
		ErrorString = qComment.GetErrorString();
	}
	return Result;
}
//-----------------------------------------------------------------------------
bool CGHelper::CommentField(PMetaTable *MetaTable, PMetaField *MetaField, QString &ErrorString)
{
	QString CommentText = ISSystem::VariantMapToJsonString(
	{
		{ "UID", MetaField->UID },
		{ "Name", MetaField->Name },
		{ "Type", ISMetaData::Instance().GetTypeDB(MetaField->Type) },
		{ "Size", MetaField->Size },
		{ "DefaultValue", MetaField->DefaultValue },
		{ "LabelName", MetaField->LabelName },
		{ "LocalListName", MetaField->LocalListName },
		{ "NotNull", MetaField->NotNull },
		{ "ReadOnly", MetaField->ReadOnly },
		{ "HideFromObject", MetaField->HideFromObject },
		{ "HideFromList", MetaField->HideFromList },
		{ "Hint", MetaField->Hint },
		{ "IsSystem", MetaField->IsSystem }
	});

	ISQuery qComment;
	qComment.SetShowLongQuery(false);
	bool Result = qComment.Execute(QString("COMMENT ON COLUMN public.%1.%2 IS '%3'").arg(MetaTable->Name.toLower()).arg(MetaTable->Alias + '_' + MetaField->Name.toLower()).arg(CommentText));
	if (!Result)
	{
		ErrorString = qComment.GetErrorString();
	}
	return Result;
}
//-----------------------------------------------------------------------------
