#include "CGIndex.h"
#include "ISQuery.h"
#include "ISSystem.h"
#include "PMetaClassForeign.h"
#include "ISMetaData.h"
#include "CGHelper.h"
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
	if (Index->GetUnique())
	{
		IndexUnique = "UNIQUE";
	}

	QString SqlText = QC_INDEX;
	SqlText = SqlText.arg(IndexUnique);
	SqlText = SqlText.arg(GetIndexName(Index));
	SqlText = SqlText.arg(Index->GetTableName());

	QString Fields = QString();

	if (Index->GetFields().count())
	{
		for (int i = 0; i < Index->GetFields().count(); ++i)
		{
			Fields += Index->GetAlias() + '_' + Index->GetFields().at(i) + ", ";
		}

		ISSystem::RemoveLastSymbolFromString(Fields, 2);
		SqlText = SqlText.arg(Fields);
	}
	else
	{
		SqlText = SqlText.arg(Index->GetAlias().toLower() + '_' + Index->GetFieldName().toLower());
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
	qSelectIndex.BindValue(":TableName", Index->GetTableName().toLower());
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
		if (Index->GetTableName().toLower() == MetaForeign->GetForeignClass().toLower())
		{
			if (Index->GetFieldName().toLower() == MetaForeign->GetForeginField().toLower())
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

	if (Index->GetFields().count())
	{
		IndexName += Index->GetTableName() + '_';
		for (int i = 0; i < Index->GetFields().count(); ++i)
		{
			IndexName += Index->GetFields().at(i) + '_';
		}

		ISSystem::RemoveLastSymbolFromString(IndexName);
	}
	else
	{
		IndexName = Index->GetTableName() + '_' + Index->GetAlias() + '_' + Index->GetFieldName() + "_index";
	}

	
	return IndexName.toLower();
}
//-----------------------------------------------------------------------------
