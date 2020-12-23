#include "ISColumnSizer.h"
#include "ISAlgorithm.h"
//-----------------------------------------------------------------------------
ISColumnSizer::ISColumnSizer()
{

}
//-----------------------------------------------------------------------------
ISColumnSizer::~ISColumnSizer()
{
	
}
//-----------------------------------------------------------------------------
ISColumnSizer& ISColumnSizer::Instance()
{
	static ISColumnSizer ColumnSizer;
	return ColumnSizer;
}
//-----------------------------------------------------------------------------
void ISColumnSizer::Initialize(const QVariantMap &VariantMap)
{
	for (const auto &TableItem : VariantMap.toStdMap())
	{
		ISStringToIntMap StringToIntMap;
		for (const auto &ColumnItem : TableItem.second.toMap().toStdMap())
		{
			StringToIntMap[ColumnItem.first] = ColumnItem.second.toInt();
		}
		Tables[TableItem.first] = StringToIntMap;
	}
}
//-----------------------------------------------------------------------------
QVariantMap ISColumnSizer::GetColumnSize() const
{
	QVariantMap VariantMap;
	for (const auto &TableItem : Tables)
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
	//Если такой таблицы ещё нет - добавляем
	if (Tables.find(TableName) == Tables.end())
	{
		Tables[TableName] = ISStringToIntMap();
	}
	Tables[TableName][FieldName] = Size;
}
//-----------------------------------------------------------------------------
int ISColumnSizer::GetColumnSize(const QString &TableName, const QString &FieldName) const
{
	std::map<QString, ISStringToIntMap>::const_iterator TableIt = Tables.find(TableName);
	if (TableIt != Tables.end()) //Нашли таблицу
	{
		ISStringToIntMap::const_iterator ColumnIt = TableIt->second.find(FieldName);
		if (ColumnIt != TableIt->second.end())
		{
			return ColumnIt->second;
		}
	}
	return 0;
}
//-----------------------------------------------------------------------------
