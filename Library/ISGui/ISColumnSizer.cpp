#include "ISColumnSizer.h"
#include "ISQuery.h"
#include "ISAlgorithm.h"
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
