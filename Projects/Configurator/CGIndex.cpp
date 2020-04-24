#include "CGIndex.h"
#include "ISQuery.h"
#include "ISSystem.h"
#include "ISMetaData.h"
//-----------------------------------------------------------------------------
static QString QS_INDEXES = PREPARE_QUERY("SELECT COUNT(*) FROM pg_indexes WHERE schemaname = current_schema() AND tablename = :TableName AND indexname = :IndexName;");
//-----------------------------------------------------------------------------
static QString QD_INDEX = "DROP INDEX public.%1;";
//-----------------------------------------------------------------------------
static QString QC_INDEX = "CREATE %1 INDEX %2 ON public.%3 USING btree(%4);";
//-----------------------------------------------------------------------------
static QString Q_REINDEX = "REINDEX INDEX %1";
//-----------------------------------------------------------------------------
bool CGIndex::CreateIndex(PMetaIndex *Index, QString &ErrorString)
{
	QString IndexUnique = Index->Unique ? "UNIQUE" : QString();
	QString Fields, SqlText = QC_INDEX.arg(IndexUnique).arg(GetIndexName(Index)).arg(Index->TableName);
	if (!Index->Fields.empty())
	{
        for (const QString &String : Index->Fields)
		{
            Fields += Index->Alias + '_' + String + ", ";
		}
		Fields.chop(2);
		SqlText = SqlText.arg(Fields);
	}
	else
	{
		SqlText = SqlText.arg(Index->Alias.toLower() + '_' + Index->FieldName.toLower());
	}

	ISQuery qCreateIndex;
	qCreateIndex.SetShowLongQuery(false);
	bool Result = qCreateIndex.Execute(SqlText);
	if (!Result)
	{
		ErrorString = qCreateIndex.GetErrorString();
	}
	return Result;
}
//-----------------------------------------------------------------------------
bool CGIndex::UpdateIndex(PMetaIndex *Index, QString &ErrorString)
{
	ISQuery qDelete;
	qDelete.SetShowLongQuery(false);
	bool Result = qDelete.Execute(QD_INDEX.arg(GetIndexName(Index)));
	if (Result)
	{
		Result = CreateIndex(Index, ErrorString);
	}
	else
	{
		ErrorString = qDelete.GetErrorString();
	}
	return Result;
}
//-----------------------------------------------------------------------------
bool CGIndex::CheckExistIndex(PMetaIndex *Index, bool &Exist, QString &ErrorString)
{
	ISQuery qSelectIndex(QS_INDEXES);
	qSelectIndex.SetShowLongQuery(false);
	qSelectIndex.BindValue(":TableName", Index->TableName.toLower());
	qSelectIndex.BindValue(":IndexName", GetIndexName(Index));
	bool Result = qSelectIndex.ExecuteFirst();
	if (Result)
	{
		Exist = qSelectIndex.ReadColumn("count").toInt() > 0;
	}
	else
	{
		ErrorString = qSelectIndex.GetErrorString();
	}
	return Result;
}
//-----------------------------------------------------------------------------
bool CGIndex::CheckIndexForeign(PMetaIndex *Index)
{
	bool Result = true;
	std::vector<PMetaForeign*> Foreigns = ISMetaData::GetInstanse().GetForeigns();
    for (PMetaForeign *MetaForeign : Foreigns)
	{
		Result = Index->TableName.toLower() == MetaForeign->ForeignClass.toLower();
		if (Result)
		{
			Result = Index->FieldName.toLower() == MetaForeign->ForeignField.toLower();
			if (Result)
			{
				break;
			}
		}
	}
	return Result;
}
//-----------------------------------------------------------------------------
bool CGIndex::ReindexIndex(PMetaIndex *Index, QString &ErrorString)
{
	ISQuery qReindex;
	qReindex.SetShowLongQuery(false);
	bool Result = qReindex.Execute(Q_REINDEX.arg(GetIndexName(Index)));
	if (!Result)
	{
		ErrorString = qReindex.GetErrorString();
	}
	return Result;
}
//-----------------------------------------------------------------------------
QString CGIndex::GetIndexName(PMetaIndex *Index)
{
	QString IndexName;
	if (!Index->Fields.empty())
	{
		IndexName += Index->TableName + '_';
        for (const QString &String : Index->Fields)
		{
            IndexName += String + '_';
		}
		IndexName.chop(1);
	}
	else
	{
		IndexName = Index->TableName + '_' + Index->Alias + '_' + Index->FieldName + "_index";
	}
	return IndexName.toLower();
}
//-----------------------------------------------------------------------------
