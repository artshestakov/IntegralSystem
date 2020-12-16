#include "ISQueryModel.h"
#include "ISQuery.h"
#include "ISMetaData.h"
#include "ISConstants.h"
#include "ISAlgorithm.h"
//-----------------------------------------------------------------------------
ISQueryModel::ISQueryModel(PMetaTable *meta_table, ISNamespace::QueryModelType model_type, QObject *parent)
	: QObject(parent),
	MetaTable(meta_table),
	ModelType(model_type),
	QuerySelectFrom("FROM " + MetaTable->Name.toLower() + SYMBOL_SPACE + MetaTable->Alias + " \n"),
	SortingField("ID"),
	SortingOrder(Qt::AscendingOrder),
	SortingIsForeign(false),
    Limit(0),
    Offset(0),
    ClassFilter(meta_table->ClassFilter)
{
	for (size_t i = 0, c = MetaTable->Fields.size(); i < c; ++i)
	{
		PMetaField *MetaField = MetaTable->Fields[i];
		if (ModelType == ISNamespace::QMT_Object) //Если тип модели - объектный
		{
			if (MetaField->HideFromObject)
			{
				continue;
			}
		}
		else if (ModelType == ISNamespace::QMT_List) //Тип модели - листовый
		{
			if (MetaField->HideFromList)
			{
				continue;
			}
		}

		if (MetaField->Foreign) //Если на поле установлен внешний ключ
		{
			PMetaTable *MetaTableForeign = ISMetaData::Instance().GetMetaTable(MetaField->Foreign->ForeignClass);
			QuerySelectLeftJoin += "LEFT JOIN " + MetaTableForeign->Name.toLower() + SYMBOL_SPACE + GetAliasForLeftJoinTable(MetaTableForeign->Alias, i) + " ON " + MetaTable->Alias + SYMBOL_POINT + MetaTable->Alias + '_' + MetaField->Name.toLower() + " = " + GetAliasForLeftJoinTable(MetaTableForeign->Alias, i) + SYMBOL_POINT + MetaTableForeign->Alias + '_' + MetaField->Foreign->ForeignField.toLower() + " \n";

			QString Temp = GetForeignViewNameField(MetaTableForeign->Alias, MetaField->Foreign, i).toLower();
			ForeignFields.emplace(MetaField->Name, Temp);

			QuerySelectFields += Temp + " AS \"" + MetaField->Name + "\", \n";
		}
		else
		{
			if (!MetaField->QueryText.isEmpty())
			{
				QuerySelectFields += '(' + MetaField->QueryText + ") AS \"" + MetaField->Name + "\", \n";
			}
			else
			{
				QuerySelectFields += MetaTable->Alias + SYMBOL_POINT + MetaTable->Alias + '_' + MetaField->Name.toLower();
				if (MetaField->Type == ISNamespace::FT_File && ModelType == ISNamespace::QMT_List)
				{
					QuerySelectFields += " IS NULL";
				}
				QuerySelectFields += " AS \"" + MetaField->Name + "\", \n";
			}
		}
	}
	QuerySelectFields.chop(3);
	QuerySelectFields += " \n";
}
//-----------------------------------------------------------------------------
ISQueryModel::~ISQueryModel()
{

}
//-----------------------------------------------------------------------------
void ISQueryModel::SetCondition(const QVariantMap &VariantMap)
{
	Conditions = VariantMap;
}
//-----------------------------------------------------------------------------
void ISQueryModel::AddCondition(const QString &Condition, const QVariant &Value)
{
	Conditions.insert(Condition, Value);
}
//-----------------------------------------------------------------------------
void ISQueryModel::ClearConditions()
{
	Conditions.clear();
}
//-----------------------------------------------------------------------------
QVariantMap ISQueryModel::GetConditions()
{
	return Conditions;
}
//-----------------------------------------------------------------------------
QString ISQueryModel::GetQueryText()
{
	QString SqlText = "SELECT \n";
	SqlText += QuerySelectFields;
	SqlText += QuerySelectFrom;
	SqlText += QuerySelectLeftJoin;
	SqlText += QueryWhereText;

	//Если для таблицы существует фильтр
	if (!ClassFilter.isEmpty())
	{
		SqlText += "WHERE " + ClassFilter + " \n";
	}

	//Фильтр поиска
	if (!SearchFilter.isEmpty())
	{
		SqlText += "AND " + SearchFilter + " \n";
	}

	//Учитываем сортировку
	SqlText += "ORDER BY " +
		(SortingIsForeign ? SortingField : MetaTable->Alias + '.' + MetaTable->Alias + '_' + SortingField) + ' ' + 
		(SortingOrder == Qt::AscendingOrder ? "ASC" : "DESC");

	//Учитываем лимит
	if (Limit)
	{
		SqlText += QString(" \nLIMIT %1 OFFSET %2").arg(Limit).arg(Offset);
	}
	return SqlText;
}
//-----------------------------------------------------------------------------
void ISQueryModel::SetParentFilter(int ParentObjectID, const QString &FieldName)
{
	if (ParentObjectID && !FieldName.isEmpty())
	{
		ClassFilter = QString("%1_%2 = %3").arg(MetaTable->Alias).arg(FieldName).arg(ParentObjectID);
	}
}
//-----------------------------------------------------------------------------
void ISQueryModel::SetClassFilter(const QString &class_filter)
{
	if (!class_filter.isEmpty())
	{
		ClassFilter = class_filter;
	}
}
//-----------------------------------------------------------------------------
QString ISQueryModel::GetClassFilter() const
{
	return ClassFilter;
}
//-----------------------------------------------------------------------------
void ISQueryModel::ClearClassFilter()
{
	ClassFilter.clear();
	Conditions.clear();
}
//-----------------------------------------------------------------------------
void ISQueryModel::SetSearchFilter(const QString &search_filter)
{
	SearchFilter = search_filter;
}
//-----------------------------------------------------------------------------
QString ISQueryModel::GetSearchFilter() const
{
	return SearchFilter;
}
//-----------------------------------------------------------------------------
void ISQueryModel::ClearSearchFilter()
{
	SearchFilter.clear();
}
//-----------------------------------------------------------------------------
QString ISQueryModel::GetSortingField() const
{
	return SortingField;
}
//-----------------------------------------------------------------------------
Qt::SortOrder ISQueryModel::GetSortingOrder() const
{
	return SortingOrder;
}
//-----------------------------------------------------------------------------
void ISQueryModel::SetSorting(const QString &FieldName, Qt::SortOrder Order)
{
	if (SortingField != FieldName)
	{
		PMetaField *MetaField = MetaTable->GetField(FieldName);
		QString FieldQueryText = MetaField->QueryText;
		if (!FieldQueryText.isEmpty()) //Если сортируемое поле является виртуальным - сортировать по запросу поля
		{
			SortingField = '(' + FieldQueryText + ')';
		}
		else
		{
			SortingIsForeign = MetaField->Foreign;
			SortingField = SortingIsForeign ? ForeignFields[FieldName] : FieldName;
		}
	}
	SortingOrder = Order;
}
//-----------------------------------------------------------------------------
void ISQueryModel::SetLimit(int limit)
{
	Limit = limit;
}
//-----------------------------------------------------------------------------
void ISQueryModel::SetOffset(int offset)
{
	Offset = offset;
}
//-----------------------------------------------------------------------------
QString ISQueryModel::GetForeignViewNameField(const QString &MetaTableForeignAlias, PMetaForeign *MetaForeign, size_t Iterator)
{
	QStringList StringList = MetaForeign->ForeignViewNameField.split(';');
	if (StringList.count() > 1)
	{
		QString SqlText = "concat(";
		for (const QString &String : StringList)
		{
			SqlText += GetAliasForLeftJoinTable(MetaTableForeignAlias, Iterator) + SYMBOL_POINT + MetaTableForeignAlias + '_' + String.toLower() + ", ' ', ";
		}
		SqlText.chop(7);
		SqlText += ')';
		return SqlText;
	}
	return GetAliasForLeftJoinTable(MetaTableForeignAlias, Iterator) + SYMBOL_POINT + MetaTableForeignAlias + '_' + MetaForeign->ForeignViewNameField.toLower();
}
//-----------------------------------------------------------------------------
QString ISQueryModel::GetAliasForLeftJoinTable(const QString &TableAlias, size_t Iterator)
{
	return TableAlias + QString::number(Iterator);
}
//-----------------------------------------------------------------------------
