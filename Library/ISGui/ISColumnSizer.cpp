#include "ISColumnSizer.h"
#include "ISQuery.h"
#include "ISAlgorithm.h"
#include "ISMetaUser.h"
//-----------------------------------------------------------------------------
static QString QS_COLUMN_SIZE = PREPARE_QUERY("SELECT clsz_tablename, clsz_fieldname, clsz_size "
											  "FROM _columnsize "
											  "WHERE clsz_user = :UserID");
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
QVariantMap ISColumnSizer::GetColumnSize() const
{
	QVariantMap VariantMap;
	for (const auto &TableItem : TablesNew)
	{
		QVariantMap TableMap;
		for (const auto &ColumnItem : TableItem.second)
		{
			TableMap[ColumnItem.first] = ColumnItem.second;
		}
		VariantMap[TableItem.first] = TableMap;
	}
	return VariantMap;
}
//-----------------------------------------------------------------------------
bool ISColumnSizer::Initialize()
{
	ISQuery qSelect(QS_COLUMN_SIZE);
	qSelect.BindValue(":UserID", CURRENT_USER_ID);
	bool Result = qSelect.Execute();
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
void ISColumnSizer::SetColumnSizeNew(const QString &TableName, const QString &FieldName, int Size)
{
	//Если такой таблицы ещё нет - добавляем
	if (TablesNew.find(TableName) == TablesNew.end())
	{
		TablesNew[TableName] = ISStringToIntMap();
	}
	TablesNew[TableName][FieldName] = Size;
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
