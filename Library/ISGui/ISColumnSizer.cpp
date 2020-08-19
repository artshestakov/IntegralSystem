#include "ISColumnSizer.h"
#include "ISQuery.h"
#include "ISAlgorithm.h"
#include "ISTcpQuery.h"
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
	: ErrorString(NO_ERROR_STRING)
{

}
//-----------------------------------------------------------------------------
ISColumnSizer::~ISColumnSizer()
{
	while (!Tables.empty())
	{
		delete ISAlgorithm::MapTakeFront<QString, ISColumnSizeItem*>(Tables);
	}
}
//-----------------------------------------------------------------------------
ISColumnSizer& ISColumnSizer::Instance()
{
	static ISColumnSizer ColumnSizer;
	return ColumnSizer;
}
//-----------------------------------------------------------------------------
QString ISColumnSizer::GetErrorString() const
{
	return ErrorString;
}
//-----------------------------------------------------------------------------
bool ISColumnSizer::Initialize(bool UseProtocol)
{
	bool Result = true;
	if (UseProtocol)
	{
		ISTcpQuery qColumnSizer(API_GET_META_DATA);
		Result = qColumnSizer.Execute();
		if (Result)
		{
			QVariantList TableList = qColumnSizer.GetAnswer()["Tables"].toList();
			for (const QVariant &Table : TableList)
			{
				QVariantMap VariantMap = Table.toMap();
				QString TableName = VariantMap["TableName"].toString();
				QString FieldName = VariantMap["FieldName"].toString();
				int Size = VariantMap["Size"].toInt();
				if (Tables.count(TableName))
				{
					Tables[TableName]->Fields[FieldName] = Size;
				}
				else
				{
					ISColumnSizeItem *ColumnSizeItem = new ISColumnSizeItem();
					ColumnSizeItem->Fields[FieldName] = Size;
					Tables.emplace(TableName, ColumnSizeItem);
				}
			}
		}
		else
		{
			ErrorString = qColumnSizer.GetErrorString();
		}
	}
	else
	{
		ISQuery qSelect(QS_COLUMN_SIZE);
		Result = qSelect.Execute();
		if (Result)
		{
			while (qSelect.Next())
			{
				QString TableName = qSelect.ReadColumn("clsz_tablename").toString();
				QString FieldName = qSelect.ReadColumn("clsz_fieldname").toString();
				int FieldSize = qSelect.ReadColumn("clsz_size").toInt();
				if (Tables.count(TableName))
				{
					Tables[TableName]->Fields[FieldName] = FieldSize;
				}
				else
				{
					ISColumnSizeItem *ColumnSizeItem = new ISColumnSizeItem();
					ColumnSizeItem->Fields[FieldName] = FieldSize;
					Tables.emplace(TableName, ColumnSizeItem);
				}
			}
		}
		else
		{
			ErrorString = qSelect.GetErrorString();
		}
	}
	return Result;
}
//-----------------------------------------------------------------------------
bool ISColumnSizer::Save()
{
	bool Result = true;
	for (const auto &TableItem : Tables)
	{
		if (TableItem.second->ModificationFlag)
		{
			for (const auto &FieldItem : TableItem.second->Fields)
			{
				ISQuery qSelect(QS_COLUMN_SIZE_COUNT);
				qSelect.BindValue(":TableName", TableItem.first);
				qSelect.BindValue(":FieldName", FieldItem.first);
				Result = qSelect.ExecuteFirst();
				if (!Result)
				{
					ErrorString = qSelect.GetErrorString();
					break;
				}

				ISQuery qUpsert(qSelect.ReadColumn("count").toInt() ? QU_COLUMN_SIZE : QI_COLUMN_SIZE);
				qUpsert.BindValue(":TableName", TableItem.first);
				qUpsert.BindValue(":FieldName", FieldItem.first);
				qUpsert.BindValue(":Size", FieldItem.second);
				Result = qUpsert.Execute();
				if (!Result)
				{
					ErrorString = qUpsert.GetErrorString();
					break;
				}
			}

			if (!Result)
			{
				break;
			}
		}
	}

	return Result;
}
//-----------------------------------------------------------------------------
bool ISColumnSizer::Clear()
{
	ISQuery qDelete(QD_COLUMN_SIZE);
	bool Result = qDelete.Execute();
	if (!Result)
	{
		ErrorString = qDelete.GetErrorString();
	}
	return Result;
}
//-----------------------------------------------------------------------------
void ISColumnSizer::SetColumnSize(const QString &TableName, const QString &FieldName, int Size)
{
	ISColumnSizeItem *ColumnSizeItem = Tables[TableName];
	if (!ColumnSizeItem)
	{
		ColumnSizeItem = new ISColumnSizeItem();
		Tables[TableName] = ColumnSizeItem;
	}
	ColumnSizeItem->Fields[FieldName] = Size;
	ColumnSizeItem->ModificationFlag = true;
}
//-----------------------------------------------------------------------------
int ISColumnSizer::GetColumnSize(const QString &TableName, const QString &FieldName) const
{
	std::map<QString, ISColumnSizeItem*>::const_iterator It = Tables.find(TableName);
	if (It != Tables.end())
	{
		return It->second->Fields[FieldName];
	}
	return 0;
}
//-----------------------------------------------------------------------------
