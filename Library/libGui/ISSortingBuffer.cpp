#include "ISSortingBuffer.h"
#include "ISQuery.h"
#include "ISAlgorithm.h"
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
static QString QD_SORTING = PREPARE_QUERY("DELETE FROM _sortingtables WHERE sgts_user = currentuserid()");
//-----------------------------------------------------------------------------
ISSortingBuffer::ISSortingBuffer()
	: ErrorString(NO_ERROR_STRING)
{
	Initialize();
}
//-----------------------------------------------------------------------------
ISSortingBuffer::~ISSortingBuffer()
{
	while (!Sortings.empty())
	{
		delete VectorTakeBack<ISSortingMetaTable *>(Sortings);
	}
}
//-----------------------------------------------------------------------------
ISSortingBuffer& ISSortingBuffer::Instance()
{
	static ISSortingBuffer SortingBuffer;
	return SortingBuffer;
}
//-----------------------------------------------------------------------------
QString ISSortingBuffer::GetErrorString() const
{
	return ErrorString;
}
//-----------------------------------------------------------------------------
void ISSortingBuffer::AddSorting(const QString &TableName, const QString &FieldName, Qt::SortOrder Sorting)
{
	if (!Sortings.empty())
	{
		for (ISSortingMetaTable *MetaSorting : Sortings)
		{
			if (MetaSorting->TableName == TableName)
			{
				if (MetaSorting->FieldName != FieldName || MetaSorting->Order != Sorting)
				{
					MetaSorting->FieldName = FieldName;
					MetaSorting->Order = Sorting;
					MetaSorting->ModificationFlag = true;
					return;
				}
			}
		}
		Sortings.emplace_back(CreateSorting(TableName, FieldName, Sorting));
	}
	else
	{
		Sortings.emplace_back(CreateSorting(TableName, FieldName, Sorting));
	}
}
//-----------------------------------------------------------------------------
bool ISSortingBuffer::SaveSortings()
{
	bool Result = true;
	for (ISSortingMetaTable *MetaSorting : Sortings)
	{
		if (MetaSorting->ModificationFlag)
		{
			Result = SaveSorting(MetaSorting);
			if (!Result)
			{
				break;
			}
		}
	}
	return Result;
}
//-----------------------------------------------------------------------------
bool ISSortingBuffer::Clear()
{
	ISQuery qDelete(QD_SORTING);
	bool Result = qDelete.Execute();
	if (!Result)
	{
		ErrorString = qDelete.GetErrorString();
	}
	return Result;
}
//-----------------------------------------------------------------------------
ISSortingMetaTable* ISSortingBuffer::GetSorting(const QString &TableName)
{
	for (ISSortingMetaTable *MetaSorting : Sortings)
	{
		if (MetaSorting->TableName == TableName)
		{
			return MetaSorting;
		}
	}
	return nullptr;
}
//-----------------------------------------------------------------------------
void ISSortingBuffer::Initialize()
{
	ISQuery qSelect(QS_SORTINGS);
	if (qSelect.Execute())
	{
		while (qSelect.Next())
		{
			Sortings.emplace_back(CreateSorting(qSelect.ReadColumn("sgts_tablename").toString(), qSelect.ReadColumn("sgts_fieldname").toString(), static_cast<Qt::SortOrder>(qSelect.ReadColumn("sgts_sorting").toInt())));
		}
	}
}
//-----------------------------------------------------------------------------
bool ISSortingBuffer::SaveSorting(ISSortingMetaTable *MetaSorting)
{
	ISQuery qSelectSorting(QS_SORTING_EXIST);
	qSelectSorting.BindValue(":TableName", MetaSorting->TableName);
	bool Result = qSelectSorting.ExecuteFirst();
	if (Result)
	{
		ISQuery qUpsert(qSelectSorting.ReadColumn("count").toInt() > 0 ? QU_SORTING : QI_SORTING);
		qUpsert.BindValue(":TableName", MetaSorting->TableName);
		qUpsert.BindValue(":FieldName", MetaSorting->FieldName);
		qUpsert.BindValue(":Sorting", MetaSorting->Order);
		Result = qUpsert.Execute();
		if (!Result)
		{
			ErrorString = qUpsert.GetErrorString();
		}
	}
	return Result;
}
//-----------------------------------------------------------------------------
ISSortingMetaTable* ISSortingBuffer::CreateSorting(const QString &TableName, const QString &FieldName, Qt::SortOrder Sorting)
{
	return new ISSortingMetaTable{ TableName, FieldName, Sorting };
}
//-----------------------------------------------------------------------------
