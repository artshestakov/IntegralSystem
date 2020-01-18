#include "ISQueryModel.h"
#include "ISDefines.h"
#include "ISQuery.h"
#include "ISMetaData.h"
#include "ISDebug.h"
#include "ISSystem.h"
//-----------------------------------------------------------------------------
ISQueryModel::ISQueryModel(PMetaClassTable *meta_table, ISNamespace::QueryModelType model_type, QObject *parent)
	: QObject(parent),
	MetaTable(meta_table),
	ModelType(model_type),
	ClassAlias(meta_table->GetAlias()),
	ClassFilter(meta_table->GetClassFilter()),
	Limit(0),
	Offset(0),
	VisibleIsDeleted(false),
	PeriodType(ISNamespace::PT_UnknownDate),
	QuerySelectText("SELECT \n" + ClassAlias + "." + ClassAlias + "_" + MetaTable->GetSystemFields().at(0)->GetName().toLower() + " AS \"" + MetaTable->GetSystemFields().at(0)->GetName() + "\", \n"),
	QuerySelectFrom("FROM " + MetaTable->GetName().toLower() + SYMBOL_SPACE + ClassAlias + " \n"),
	QueryWhereText("WHERE \n"),
	OrderFieldDefault(ClassAlias + "." + ClassAlias + "_id"),
	OrderSort(Qt::AscendingOrder)
{
	CreateQuerySelectSystemFields();
	CreateQuerySelectFields();
	CreateQuerySelectIsDeleted();

	QueryWhereText += QuerySelectIsDeleted; //???
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
	QString SqlText;
	SqlText += QuerySelectText;
	SqlText += QuerySelectSystemFields;
	SqlText += QuerySelectFields;
	SqlText += QuerySelectFrom;
	SqlText += QuerySelectLeftJoin;

	if (VisibleIsDeleted)
	{
		SqlText += QueryWhereText + " IN(true, false) \n";
	}
	else
	{
		if (ModelType == ISNamespace::QMT_List)
		{
			SqlText += QueryWhereText + " = false \n";
		}
		else if (ModelType == ISNamespace::QMT_Object)
		{
			SqlText += QueryWhereText + " IN(true, false) \n";
		}
	}

	//Если задан период отображения
	if (PeriodRange.IsValid())
	{
		QString PeriodString = QString("AND " + ClassAlias + "." + ClassAlias + "_%1 BETWEEN '%2' AND '%3' \n");
		switch (PeriodType)
		{
		case ISNamespace::PT_CreationDate: PeriodString = PeriodString.arg("creationdate"); break;
		case ISNamespace::PT_UpdationDate: PeriodString = PeriodString.arg("updationdate"); break;
		}
		SqlText += PeriodString.arg(QDateTime(PeriodRange.BeginValue.toDate(), QTime(0, 0)).toString(DATE_TIME_FORMAT_V7)).arg(QDateTime(PeriodRange.EndValue.toDate(), QTime(23, 59, 59)).toString(DATE_TIME_FORMAT_V7));
	}

	//Если для таблицы существует фильтр
	if (ClassFilter.length())
	{
		SqlText += "AND " + ClassFilter + " \n";
	}

	//Фильтр поиска
	if (SearchFilter.length())
	{
		SqlText += "AND " + SearchFilter + " \n";
	}

	//Учитывание сортировки
	QString Sort;
	if (OrderSort == Qt::AscendingOrder)
	{
		Sort = " ASC";
	}
	else
	{
		Sort = " DESC";
	}

	if (OrderField.isEmpty())
	{
		QueryOrderText = "ORDER BY " + OrderFieldDefault + Sort;
	}
	else
	{
		QueryOrderText = "ORDER BY " + OrderField + Sort;
	}

	SqlText += QueryOrderText;
	if (Limit)
	{
		SqlText += QString(" \nLIMIT %1 OFFSET %2").arg(Limit).arg(Offset);
	}

	return SqlText;
}
//-----------------------------------------------------------------------------
bool ISQueryModel::GetVisibleIsDeleted() const
{
	return VisibleIsDeleted;
}
//-----------------------------------------------------------------------------
void ISQueryModel::SetVisibleIsDeleted(bool Visible)
{
	VisibleIsDeleted = Visible;
}
//-----------------------------------------------------------------------------
void ISQueryModel::SetParentObjectIDClassFilter(int ParentObjectID)
{
	if (ClassFilter.contains(":ParentObjectID"))
	{
		ClassFilter.replace(":ParentObjectID", QString::number(ParentObjectID));
	}
}
//-----------------------------------------------------------------------------
ISNamespace::PeriodType ISQueryModel::GetPeriodType() const
{
	return PeriodType;
}
//-----------------------------------------------------------------------------
ISRangeStruct ISQueryModel::GetPeriod() const
{
	return PeriodRange;
}
//-----------------------------------------------------------------------------
void ISQueryModel::SetPeriod(ISNamespace::PeriodType period_type, const ISRangeStruct &period_range)
{
	PeriodType = period_type;
	PeriodRange = period_range;
}
//-----------------------------------------------------------------------------
void ISQueryModel::ClearPeriod()
{
	PeriodType = ISNamespace::PT_UnknownDate;
	PeriodRange.Clear();
}
//-----------------------------------------------------------------------------
void ISQueryModel::SetClassFilter(const QString &class_filter)
{
	if (class_filter.length())
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
void ISQueryModel::SetOrderField(const QString &FullFieldName, const QString &FieldName)
{
	PMetaClassField *MetaField = MetaTable->GetField(FieldName);
	QString FieldQueryText = MetaField->GetQueryText();
	if (FieldQueryText.length()) //Если сортируемое поле является виртуальным - сортировать по запросу поля
	{
		OrderField = "(" + FieldQueryText + ")";
	}
	else
	{
		PMetaClassForeign *MetaForeign = MetaField->GetForeign();
		if (MetaForeign)
		{
			OrderField = ForeignFields.value(FieldName);
		}
		else
		{
			OrderField = FullFieldName;
		}
	}
}
//-----------------------------------------------------------------------------
void ISQueryModel::SetOrderSort(Qt::SortOrder order_sort)
{
	OrderSort = order_sort;
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
void ISQueryModel::CreateQuerySelectSystemFields()
{
	for (int i = 1; i < MetaTable->GetSystemFields().count(); i++) //Обход системных полей и включение их в запрос
	{
		PMetaClassField *SystemField = MetaTable->GetSystemFields().at(i);

		if (!MetaTable->GetVisibleSystemFields().contains(SystemField))
		{
			if (SystemField->GetHideFromList())
			{
				continue;
			}
		}

		QuerySelectSystemFields += ClassAlias + "." + ClassAlias + "_" + SystemField->GetName().toLower() + " AS \"" + SystemField->GetName() + "\", \n";
	}
}
//-----------------------------------------------------------------------------
void ISQueryModel::CreateQuerySelectFields()
{
	for (int i = 0; i < MetaTable->GetFields().count(); i++)
	{
		PMetaClassField *Field = MetaTable->GetFields().at(i);

		if (ModelType == ISNamespace::QMT_Object)
		{
			if (Field->GetHideFromObject())
			{
				continue;
			}
		}
		else if (ModelType == ISNamespace::QMT_List)
		{
			if (Field->GetHideFromList())
			{
				continue;
			}
		}

		if (Field->GetForeign()) //Если на поле установлен внешний ключ
		{
			PMetaClassForeign *MetaForeign = Field->GetForeign();
			PMetaClassTable *MetaTableForeign = ISMetaData::GetInstanse().GetMetaTable(MetaForeign->GetForeignClass());
			
			QuerySelectLeftJoin += "LEFT JOIN " + MetaTableForeign->GetName().toLower() + SYMBOL_SPACE + ISQueryModelHelper::GetAliasForLeftJoinTable(MetaTableForeign->GetAlias(), i) + " ON " + ClassAlias + "." + ClassAlias + "_" + Field->GetName().toLower() + " = " + ISQueryModelHelper::GetAliasForLeftJoinTable(MetaTableForeign->GetAlias(), i) + "." + MetaTableForeign->GetAlias() + "_" + MetaForeign->GetForeginField().toLower() + " \n";
			
			QString Temp = ISQueryModelHelper::GetForeignViewNameField(MetaTableForeign->GetAlias(), MetaForeign, i).toLower();
			ForeignFields.insert(Field->GetName(), Temp);

			QuerySelectFields += Temp + " AS \"" + Field->GetName() + "\", \n";
		}
		else
		{
			if (Field->GetQueryText().length())
			{
				QuerySelectFields += "(" + Field->GetQueryText() + ") AS \"" + Field->GetName() + "\", \n";
			}
			else
			{
				QuerySelectFields += ClassAlias + "." + ClassAlias + "_" + Field->GetName().toLower() + " AS \"" + Field->GetName() + "\", \n";
			}
		}
	}

	ISSystem::RemoveLastSymbolFromString(QuerySelectFields, 3);
	QuerySelectFields += " \n";
}
//-----------------------------------------------------------------------------
void ISQueryModel::CreateQuerySelectIsDeleted()
{
	QuerySelectIsDeleted = ClassAlias + "." + ClassAlias + "_isdeleted";
}
//-----------------------------------------------------------------------------
void ISQueryModel::CheckQuery(const QString &QueryText)
{
	ISQuery qQuery;
	if (qQuery.Prepare(QueryText))
	{
		ISDebug::ShowDebugString("Prepare Query - OK");
	}
	else
	{
		ISDebug::ShowCriticalString("Prepare Query - ERROR");
	}
}
//-----------------------------------------------------------------------------
