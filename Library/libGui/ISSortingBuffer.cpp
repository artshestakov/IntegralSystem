#include "StdAfx.h"
#include "ISSortingBuffer.h"
#include "ISQuery.h"
#include "ISCountingTime.h"
#include "ISDebug.h"
#include "ISSystem.h"
//-----------------------------------------------------------------------------
static QString QS_SORTINGS = PREPARE_QUERY("SELECT sgts_tablename, sgts_fieldname, sgts_sorting "
										   "FROM _sortingtables "
										   "WHERE NOT sgts_isdeleted "
										   "AND sgts_user = currentuserid()");
//-----------------------------------------------------------------------------
static QString QS_SORTING_EXIST = PREPARE_QUERY("SELECT COUNT(*) "
												"FROM _sortingtables "
												"WHERE NOT sgts_isdeleted "
												"AND sgts_user = currentuserid() "
												"AND sgts_tablename = :TableName");
//-----------------------------------------------------------------------------
static QString QU_SORTING = PREPARE_QUERY("UPDATE _sortingtables SET "
										  "sgts_fieldname = :FieldName, "
										  "sgts_sorting = :Sorting "
										  "WHERE sgts_user = currentuserid() "
										  "AND sgts_tablename = :TableName");
//-----------------------------------------------------------------------------
static QString QI_SORTING = PREPARE_QUERY("INSERT INTO _sortingtables(sgts_tablename, sgts_fieldname, sgts_sorting) "
										  "VALUES(:TableName, :FieldName, :Sorting)");
//-----------------------------------------------------------------------------
ISSortingBuffer::ISSortingBuffer() : QObject()
{
	Initialize();
}
//-----------------------------------------------------------------------------
ISSortingBuffer::~ISSortingBuffer()
{

}
//-----------------------------------------------------------------------------
ISSortingBuffer& ISSortingBuffer::GetInstance()
{
	static ISSortingBuffer SortingBuffer;
	return SortingBuffer;
}
//-----------------------------------------------------------------------------
void ISSortingBuffer::AddSorting(const QString &TableName, const QString &FieldName, int Sorting)
{
	if (Sortings.count())
	{
		for (ISSortingMetaTable *MetaSorting : Sortings)
		{
			if (MetaSorting->GetTableName() == TableName)
			{
				if (MetaSorting->GetFieldName() != FieldName || MetaSorting->GetSortingType() != Sorting)
				{
					MetaSorting->SetFieldName(FieldName);
					MetaSorting->SetSortingType(static_cast<Qt::SortOrder>(Sorting));
					MetaSorting->SetModificationFlag(true);
					return;
				}
			}
		}

		Sortings.append(CreateSorting(TableName, FieldName, Sorting));
	}
	else
	{
		Sortings.append(CreateSorting(TableName, FieldName, Sorting));
	}
}
//-----------------------------------------------------------------------------
void ISSortingBuffer::SaveSortings()
{
	for (ISSortingMetaTable *MetaSorting : Sortings)
	{
		if (MetaSorting->GetModificationFlag())
		{
			SaveSorting(MetaSorting);
		}
	}
}
//-----------------------------------------------------------------------------
ISSortingMetaTable* ISSortingBuffer::GetSorting(const QString &TableName)
{
	for (int i = 0; i < Sortings.count(); ++i)
	{
		ISSortingMetaTable *MetaSorting = Sortings.at(i);
		if (MetaSorting->GetTableName() == TableName)
		{
			return MetaSorting;
		}
	}

	return nullptr;
}
//-----------------------------------------------------------------------------
void ISSortingBuffer::Initialize()
{
	ISCountingTime CountingTime;

	ISQuery qSelect(QS_SORTINGS);
	if (qSelect.Execute())
	{
		while (qSelect.Next())
		{
			QString TableName = qSelect.ReadColumn("sgts_tablename").toString();
			QString FieldName = qSelect.ReadColumn("sgts_fieldname").toString();
			int Sorting = qSelect.ReadColumn("sgts_sorting").toInt();

			Sortings.append(CreateSorting(TableName, FieldName, Sorting));
		}
	}

	ISDebug::ShowDebugString(QString("Initialized SortingBuffer %1 msec").arg(CountingTime.GetElapsed()));
}
//-----------------------------------------------------------------------------
void ISSortingBuffer::SaveSorting(ISSortingMetaTable *MetaSorting)
{
	ISQuery qSelectSorting(QS_SORTING_EXIST);
	qSelectSorting.BindValue(":TableName", MetaSorting->GetTableName());
	if (qSelectSorting.ExecuteFirst())
	{
		int Count = qSelectSorting.ReadColumn("count").toInt();
		if (Count)
		{
			ISQuery qUpdateSorting(QU_SORTING);
			qUpdateSorting.BindValue(":FieldName", MetaSorting->GetFieldName());
			qUpdateSorting.BindValue(":Sorting", MetaSorting->GetSortingType());
			qUpdateSorting.BindValue(":TableName", MetaSorting->GetTableName());
			qUpdateSorting.Execute();
		}
		else
		{
			ISQuery qInsertSorting(QI_SORTING);
			qInsertSorting.BindValue(":TableName", MetaSorting->GetTableName());
			qInsertSorting.BindValue(":FieldName", MetaSorting->GetFieldName());
			qInsertSorting.BindValue(":Sorting", MetaSorting->GetSortingType());
			qInsertSorting.Execute();
		}
	}
}
//-----------------------------------------------------------------------------
ISSortingMetaTable* ISSortingBuffer::CreateSorting(const QString &TableName, const QString &FieldName, int Sorting)
{
	ISSortingMetaTable *MetaSorting = new ISSortingMetaTable();
	MetaSorting->SetTableName(TableName);
	MetaSorting->SetFieldName(FieldName);
	MetaSorting->SetSortingType(static_cast<Qt::SortOrder>(Sorting));
	return MetaSorting;
}
//-----------------------------------------------------------------------------
