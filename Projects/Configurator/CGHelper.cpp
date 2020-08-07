#include "CGHelper.h"
#include "ISQuery.h"
#include "ISSystem.h"
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
	}).toUtf8();

	ISQuery qComment;
	qComment.SetShowLongQuery(false);
	bool Result = qComment.Execute(QString("COMMENT ON TABLE public.%1 IS '%2'").arg(MetaTable->Name).arg(CommentText));
	if (!Result)
	{
		ErrorString = qComment.GetErrorString();
	}
	return Result;
}
//-----------------------------------------------------------------------------
bool CGHelper::CommentField(const QString &TableName, const QString &FieldName, const QString &Description, QString &ErrorString)
{
	ISQuery qComment;
	qComment.SetShowLongQuery(false);
	bool Result = qComment.Execute(QString("COMMENT ON COLUMN public.%1.%2 IS '%3'").arg(TableName).arg(FieldName).arg(Description));
	if (!Result)
	{
		ErrorString = qComment.GetErrorString();
	}
	return Result;
}
//-----------------------------------------------------------------------------
