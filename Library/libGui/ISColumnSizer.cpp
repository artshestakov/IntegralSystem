#include "ISColumnSizer.h"
#include "ISQuery.h"
//-----------------------------------------------------------------------------
static QString QS_COLUMN_SIZE = PREPARE_QUERY("SELECT clsz_tablename, clsz_fieldname, clsz_size "
											  "FROM _columnsize "
											  "WHERE clsz_user = currentuserid()");
//-----------------------------------------------------------------------------
static QString QS_COLUMN_SIZE_COUNT = PREPARE_QUERY("SELECT COUNT(*) "
													"FROM _columnsize "
													"WHERE clsz_user = currentuserid() "
													"AND clsz_tablename = :TableName "
													"AND clsz_fieldname = :FieldName");
//-----------------------------------------------------------------------------
static QString QU_COLUMN_SIZE = PREPARE_QUERY("UPDATE _columnsize SET "
											  "clsz_size = :Size "
											  "WHERE clsz_user = currentuserid() "
											  "AND clsz_tablename = :TableName "
											  "AND clsz_fieldname = :FieldName");
//-----------------------------------------------------------------------------
static QString QI_COLUMN_SIZE = PREPARE_QUERY("INSERT INTO _columnsize(clsz_tablename, clsz_fieldname, clsz_size) "
											  "VALUES (:TableName, :FieldName, :Size)");
//-----------------------------------------------------------------------------
static QString QD_COLUMN_SIZE = PREPARE_QUERY("DELETE FROM _columnsize "
											  "WHERE clsz_user = currentuserid()");
//-----------------------------------------------------------------------------
ISColumnSizer::ISColumnSizer()
{

}
//-----------------------------------------------------------------------------
ISColumnSizer::~ISColumnSizer()
{
	while (!Tables.empty())
	{
		delete Tables.take(Tables.keys().back());
	}
}
//-----------------------------------------------------------------------------
ISColumnSizer& ISColumnSizer::Instance()
{
	static ISColumnSizer ColumnSizer;
	return ColumnSizer;
}
//-----------------------------------------------------------------------------
bool ISColumnSizer::Initialize()
{
	ISQuery qSelect(QS_COLUMN_SIZE);
	bool Result = qSelect.Execute();
	if (Result)
	{
		while (qSelect.Next())
		{
			QString TableName = qSelect.ReadColumn("clsz_tablename").toString();
			QString FieldName = qSelect.ReadColumn("clsz_fieldname").toString();
			int FieldSize = qSelect.ReadColumn("clsz_size").toInt();

			if (Tables.contains(TableName))
			{
				Tables[TableName]->Fields[FieldName] = FieldSize;
			}
			else
			{
				ISColumnSizeItem *ColumnSizeItem = new ISColumnSizeItem();
				ColumnSizeItem->Fields[FieldName] = FieldSize;
				Tables.insert(TableName, ColumnSizeItem);
			}
		}
	}
	return Result;
}
//-----------------------------------------------------------------------------
void ISColumnSizer::Save()
{
	for (const auto &TableItem : Tables.toStdMap())
	{
		if (TableItem.second->ModificationFlag)
		{
			for (const auto &FieldItem : TableItem.second->Fields)
			{
				ISQuery qSelect(QS_COLUMN_SIZE_COUNT);
				qSelect.BindValue(":TableName", TableItem.first);
				qSelect.BindValue(":FieldName", FieldItem.first);
				if (qSelect.ExecuteFirst())
				{
					if (qSelect.ReadColumn("count").toInt())
					{
						ISQuery qUpdate(QU_COLUMN_SIZE);
						qUpdate.BindValue(":Size", FieldItem.second);
						qUpdate.BindValue(":TableName", TableItem.first);
						qUpdate.BindValue(":FieldName", FieldItem.first);
						qUpdate.Execute();
					}
					else
					{
						ISQuery qInsert(QI_COLUMN_SIZE);
						qInsert.BindValue(":TableName", TableItem.first);
						qInsert.BindValue(":FieldName", FieldItem.first);
						qInsert.BindValue(":Size", FieldItem.second);
						qInsert.Execute();
					}
				}
			}
		}
	}
}
//-----------------------------------------------------------------------------
void ISColumnSizer::Clear()
{
	ISQuery(QD_COLUMN_SIZE).Execute();
}
//-----------------------------------------------------------------------------
void ISColumnSizer::SetColumnSize(const QString &TableName, const QString &FieldName, int Size)
{
	ISColumnSizeItem *ColumnSizeItem = Tables.value(TableName);
	if (!ColumnSizeItem)
	{
		ColumnSizeItem = new ISColumnSizeItem();
		Tables.insert(TableName, ColumnSizeItem);
	}
	ColumnSizeItem->Fields[FieldName] = Size;
	ColumnSizeItem->ModificationFlag = true;
}
//-----------------------------------------------------------------------------
int ISColumnSizer::GetColumnSize(const QString &TableName, const QString &FieldName) const
{
	int Result = 0;
	ISColumnSizeItem *ColumnSizeItem = Tables.value(TableName);
	if (ColumnSizeItem)
	{
		Result = ColumnSizeItem->Fields[FieldName];
	}
	return Result;
}
//-----------------------------------------------------------------------------
