#include "CGHelper.h"
#include "ISQuery.h"
#include "ISQueryText.h"
//-----------------------------------------------------------------------------
static QString QS_COLUMN = PREPARE_QUERY("SELECT COUNT(*) "
										 "FROM information_schema.columns "
										 "WHERE table_catalog = current_database() "
										 "AND table_schema = current_schema() "
										 "AND table_name = :TableName "
										 "AND column_name = :ColumnName");
//-----------------------------------------------------------------------------
static QString QU_COMMENT_TABLE = "COMMENT ON TABLE public.%1 IS '%2'";
//-----------------------------------------------------------------------------
static QString QU_COMMENT_FIELD = "COMMENT ON COLUMN public.%1.%2 IS '%3'";
//-----------------------------------------------------------------------------
bool CGHelper::CheckExistColumn(PMetaClassTable *MetaTable, const QString &ColumnName)
{
	ISQuery qSelectColumn(QS_COLUMN);
	qSelectColumn.SetShowLongQuery(false);
	qSelectColumn.BindValue(":TableName", MetaTable->Name.toLower());
	qSelectColumn.BindValue(":ColumnName", ColumnName);
	qSelectColumn.ExecuteFirst();
	if (qSelectColumn.ReadColumn("count").toInt() == 1)
	{
		return true;
	}

	return false;
}
//-----------------------------------------------------------------------------
void CGHelper::CommentTable(const QString &TableName, const QString &Description)
{
	QString QueryText = QU_COMMENT_TABLE;
	QueryText = QueryText.arg(TableName);
	QueryText = QueryText.arg(Description);

	ISQuery qComment;
	qComment.SetShowLongQuery(false);
	qComment.Execute(QueryText);
}
//-----------------------------------------------------------------------------
void CGHelper::CommentField(const QString &TableName, const QString &FieldName, const QString &Description)
{
	QString QueryText = QU_COMMENT_FIELD;
	QueryText = QueryText.arg(TableName);
	QueryText = QueryText.arg(FieldName);
	QueryText = QueryText.arg(Description);

	ISQuery qComment;
	qComment.SetShowLongQuery(false);
	qComment.Execute(QueryText);
}
//-----------------------------------------------------------------------------
