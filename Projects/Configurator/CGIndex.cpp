#include "CGIndex.h"
#include "ISQuery.h"
#include "ISSystem.h"
#include "PMetaClassForeign.h"
#include "ISMetaData.h"
#include "ISQueryText.h"
//-----------------------------------------------------------------------------
static QString QS_INDEXES = PREPARE_QUERY("SELECT COUNT(*) FROM pg_indexes WHERE schemaname = current_schema() AND tablename = :TableName AND indexname = :IndexName;");
//-----------------------------------------------------------------------------
static QString QD_INDEX = "DROP INDEX public.%1;";
//-----------------------------------------------------------------------------
static QString QC_INDEX = "CREATE %1 INDEX %2 ON public.%3 USING btree(%4);";
//-----------------------------------------------------------------------------
static QString Q_REINDEX = "REINDEX INDEX %1";
//-----------------------------------------------------------------------------
bool CGIndex::CreateIndex(PMetaClassIndex *Index, QString &ErrorString)
{
	QString IndexUnique = QString();
	if (Index->Unique)
	{
		IndexUnique = "UNIQUE";
	}

	QString SqlText = QC_INDEX;
	SqlText = SqlText.arg(IndexUnique);
	SqlText = SqlText.arg(GetIndexName(Index));
	SqlText = SqlText.arg(Index->TableName);

	QString Fields = QString();

	if (Index->Fields.count())
	{
		for (int i = 0; i < Index->Fields.count(); ++i)
		{
			Fields += Index->Alias + '_' + Index->Fields.at(i) + ", ";
		}

		ISSystem::RemoveLastSymbolFromString(Fields, 2);
		SqlText = SqlText.arg(Fields);
	}
	else
	{
		SqlText = SqlText.arg(Index->Alias.toLower() + '_' + Index->FieldName.toLower());
	}

	ISQuery qCreateIndex;
	qCreateIndex.SetShowLongQuery(false);
	bool Created = qCreateIndex.Execute(SqlText);
	if (!Created)
	{
		ErrorString = qCreateIndex.GetErrorText();
	}

	return Created;
}
//-----------------------------------------------------------------------------
bool CGIndex::UpdateIndex(PMetaClassIndex *Index, QString &ErrorString)
{
	ISQuery qDelete;
	qDelete.SetShowLongQuery(false);
	if (qDelete.Execute(QD_INDEX.arg(GetIndexName(Index))))
	{
		return CreateIndex(Index, ErrorString);
	}

	return false;
}
//-----------------------------------------------------------------------------
bool CGIndex::CheckExistIndex(PMetaClassIndex *Index)
{
	ISQuery qSelectIndex(QS_INDEXES);
	qSelectIndex.SetShowLongQuery(false);
	qSelectIndex.BindValue(":TableName", Index->TableName.toLower());
	qSelectIndex.BindValue(":IndexName", GetIndexName(Index));
	if (qSelectIndex.ExecuteFirst())
	{
		int Count = qSelectIndex.ReadColumn("count").toInt();
		if (Count)
		{
			return true;
		}
	}

	return false;
}
//-----------------------------------------------------------------------------
bool CGIndex::CheckIndexForeign(PMetaClassIndex *Index)
{
	QVector<PMetaClassForeign*> Foreigns = ISMetaData::GetInstanse().GetForeigns();

	for (int i = 0; i < Foreigns.count(); ++i)
	{
		PMetaClassForeign *MetaForeign = Foreigns.at(i);
		if (Index->TableName.toLower() == MetaForeign->ForeignClass.toLower())
		{
			if (Index->FieldName.toLower() == MetaForeign->ForeignField.toLower())
			{
				return true;
			}
		}
	}

	return false;
}
//-----------------------------------------------------------------------------
bool CGIndex::ReindexIndex(PMetaClassIndex *Index, QString &ErrorString)
{
	QString QueryText = Q_REINDEX.arg(GetIndexName(Index));
	ISQuery qReindex;
	qReindex.SetShowLongQuery(false);
	bool Executed = qReindex.Execute(QueryText);

	if (!Executed)
	{
		ErrorString = qReindex.GetErrorText();
	}

	return Executed;
}
//-----------------------------------------------------------------------------
QString CGIndex::GetIndexName(PMetaClassIndex *Index)
{
	QString IndexName;
	if (Index->Fields.count())
	{
		IndexName += Index->TableName + '_';
		for (int i = 0; i < Index->Fields.count(); ++i)
		{
			IndexName += Index->Fields.at(i) + '_';
		}
		ISSystem::RemoveLastSymbolFromString(IndexName);
	}
	else
	{
		IndexName = Index->TableName + '_' + Index->Alias + '_' + Index->FieldName + "_index";
	}
	return IndexName.toLower();
}
//-----------------------------------------------------------------------------
