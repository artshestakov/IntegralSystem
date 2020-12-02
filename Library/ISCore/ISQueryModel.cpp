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
	ClassAlias(meta_table->Alias),
	QuerySelectFrom("FROM " + MetaTable->Name.toLower() + SYMBOL_SPACE + ClassAlias + " \n"),
	OrderFieldDefault(ClassAlias + SYMBOL_POINT + ClassAlias + "_id"),
	OrderSort(Qt::AscendingOrder),
    Limit(0),
    Offset(0),
    PeriodType(ISNamespace::PT_CreationDate),
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
			//Проверяем, если текущее поле не включено в отображение системных полей - проверяем нужно ли его вообще отображать
			if (!ISAlgorithm::VectorContains(MetaTable->SystemFieldsVisible, MetaField))
			{
				if (MetaField->HideFromList)
				{
					continue;
				}
			}
		}

		if (MetaField->Foreign) //Если на поле установлен внешний ключ
		{
			PMetaTable *MetaTableForeign = ISMetaData::Instance().GetMetaTable(MetaField->Foreign->ForeignClass);
			QuerySelectLeftJoin += "LEFT JOIN " + MetaTableForeign->Name.toLower() + SYMBOL_SPACE + GetAliasForLeftJoinTable(MetaTableForeign->Alias, i) + " ON " + ClassAlias + SYMBOL_POINT + ClassAlias + '_' + MetaField->Name.toLower() + " = " + GetAliasForLeftJoinTable(MetaTableForeign->Alias, i) + SYMBOL_POINT + MetaTableForeign->Alias + '_' + MetaField->Foreign->ForeignField.toLower() + " \n";

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
				QuerySelectFields += ClassAlias + SYMBOL_POINT + ClassAlias + '_' + MetaField->Name.toLower();
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

	//Если задан период отображения
	/*if (PeriodBegin.isValid() && PeriodEnd.isValid())
	{
		QString PeriodString = QString("AND " + ClassAlias + SYMBOL_POINT + ClassAlias + "_%1 BETWEEN '%2' AND '%3' \n");
		switch (PeriodType)
		{
		case ISNamespace::PT_CreationDate: PeriodString = PeriodString.arg("creationdate"); break;
		case ISNamespace::PT_UpdationDate: PeriodString = PeriodString.arg("updationdate"); break;
        default: break;
		}
		SqlText += PeriodString.arg(QDateTime(PeriodBegin, QTime(0, 0)).toString(FORMAT_DATE_TIME_V7)).arg(QDateTime(PeriodEnd, QTime(23, 59, 59)).toString(FORMAT_DATE_TIME_V7));
	}*/

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

	//Учитывание сортировки
	QString Sort = OrderSort == Qt::AscendingOrder ? " ASC" : " DESC";
	QueryOrderText = "ORDER BY " + (OrderField.isEmpty() ? OrderFieldDefault : OrderField) + Sort;
	SqlText += QueryOrderText;

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
ISNamespace::PeriodType ISQueryModel::GetPeriodType() const
{
	return PeriodType;
}
//-----------------------------------------------------------------------------
QDate ISQueryModel::GetPeriodBegin() const
{
	return PeriodBegin;
}
//-----------------------------------------------------------------------------
QDate ISQueryModel::GetPeriodEnd() const
{
	return PeriodEnd;
}
//-----------------------------------------------------------------------------
void ISQueryModel::SetPeriod(ISNamespace::PeriodType period_type, const QDate &period_begin, const QDate &period_end)
{
	PeriodType = period_type;
	PeriodBegin = period_begin;
	PeriodEnd = period_end;
}
//-----------------------------------------------------------------------------
void ISQueryModel::ClearPeriod()
{
	PeriodType = ISNamespace::PT_UnknownDate;
	PeriodBegin = QDate();
	PeriodEnd = QDate();
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
void ISQueryModel::SetOrderField(const QString &FullFieldName, const QString &FieldName, Qt::SortOrder Order)
{
	PMetaField *MetaField = MetaTable->GetField(FieldName);
	QString FieldQueryText = MetaField->QueryText;
	if (!FieldQueryText.isEmpty()) //Если сортируемое поле является виртуальным - сортировать по запросу поля
	{
		OrderField = '(' + FieldQueryText + ')';
	}
	else
	{
		OrderField = MetaField->Foreign ? ForeignFields[FieldName] : FullFieldName;
	}
	OrderSort = Order;
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
