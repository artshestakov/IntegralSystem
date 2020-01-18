#include "ISMetaViewQuery.h"
#include "ISMetaData.h"
#include "PMetaClassTable.h"
#include "PMetaClassField.h"
#include "ISSystem.h"
//-----------------------------------------------------------------------------
ISMetaViewQuery::ISMetaViewQuery(const QString &QueryName, QObject *parent) : QObject(parent)
{
	MetaQuery = ISMetaData::GetInstanse().GetMetaQuery(QueryName);

	OrderSort = Qt::AscendingOrder;
}
//-----------------------------------------------------------------------------
ISMetaViewQuery::~ISMetaViewQuery()
{

}
//-----------------------------------------------------------------------------
QString ISMetaViewQuery::GetQueryText()
{
	QString Result = "SELECT \n";

	for (int i = 0; i < MetaQuery->GetFields().count(); i++)
	{
		PMetaClassField *MetaQueryField = MetaQuery->GetFields().at(i);
		Result += MetaQueryField->GetQueryText() + " AS \"" + MetaQueryField->GetName() + "\", \n";
	}

	ISSystem::RemoveLastSymbolFromString(Result, 3);
	
	Result += " \n";
	Result += "FROM " + MetaQuery->GetParent() + " \n";

	for (int i = 0; i < MetaQuery->GetJoins().count(); i++)
	{
		Result += MetaQuery->GetJoins().at(i) + " \n";
	}

	if (MetaQuery->GetWhere().length())
	{
		Result += "WHERE " + MetaQuery->GetWhere() + " \n";
	}

	if (ClassFilter.length())
	{
		if (MetaQuery->GetWhere().length())
		{
			Result += "AND ";
		}
		else
		{
			Result += "WHERE ";
		}

		Result += ClassFilter + " \n";
	}

	if (OrderField.length())
	{
		Result += "ORDER BY " + OrderField;

		switch (OrderSort)
		{
		case Qt::AscendingOrder: Result += " ASC"; break;
		case Qt::DescendingOrder: Result += " DESC"; break;
		}
	}

	return Result;
}
//-----------------------------------------------------------------------------
void ISMetaViewQuery::SetClassFilter(const QString &class_filter)
{
	ClassFilter = class_filter;
}
//-----------------------------------------------------------------------------
void ISMetaViewQuery::ClearClassFilter()
{
	ClassFilter.clear();
}
//-----------------------------------------------------------------------------
void ISMetaViewQuery::SetOrderField(const QString &FieldName)
{
	OrderField = FieldName;
}
//-----------------------------------------------------------------------------
void ISMetaViewQuery::SetOrderSort(Qt::SortOrder order_sort)
{
	OrderSort = order_sort;
}
//-----------------------------------------------------------------------------
