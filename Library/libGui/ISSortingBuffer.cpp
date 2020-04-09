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
ISSortingBuffer::ISSortingBuffer()
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
ISSortingBuffer& ISSortingBuffer::GetInstance()
{
	static ISSortingBuffer SortingBuffer;
	return SortingBuffer;
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
void ISSortingBuffer::SaveSortings()
{
	for (ISSortingMetaTable *MetaSorting : Sortings)
	{
		if (MetaSorting->ModificationFlag)
		{
			SaveSorting(MetaSorting);
		}
	}
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
void ISSortingBuffer::SaveSorting(ISSortingMetaTable *MetaSorting)
{
	ISQuery qSelectSorting(QS_SORTING_EXIST);
	qSelectSorting.BindValue(":TableName", MetaSorting->TableName);
	if (qSelectSorting.ExecuteFirst())
	{
		if (qSelectSorting.ReadColumn("count").toInt())
		{
			ISQuery qUpdateSorting(QU_SORTING);
			qUpdateSorting.BindValue(":FieldName", MetaSorting->FieldName);
			qUpdateSorting.BindValue(":Sorting", MetaSorting->Order);
			qUpdateSorting.BindValue(":TableName", MetaSorting->TableName);
			qUpdateSorting.Execute();
		}
		else
		{
			ISQuery qInsertSorting(QI_SORTING);
			qInsertSorting.BindValue(":TableName", MetaSorting->TableName);
			qInsertSorting.BindValue(":FieldName", MetaSorting->FieldName);
			qInsertSorting.BindValue(":Sorting", MetaSorting->Order);
			qInsertSorting.Execute();
		}
	}
}
//-----------------------------------------------------------------------------
ISSortingMetaTable* ISSortingBuffer::CreateSorting(const QString &TableName, const QString &FieldName, Qt::SortOrder Sorting)
{
	//???
	ISSortingMetaTable *MetaSorting = new ISSortingMetaTable();
	MetaSorting->TableName = TableName;
	MetaSorting->FieldName = FieldName;
	MetaSorting->Order = Sorting;
	return MetaSorting;
}
//-----------------------------------------------------------------------------
