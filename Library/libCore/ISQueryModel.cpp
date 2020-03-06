#include "ISQueryModel.h"
#include "ISQuery.h"
#include "ISMetaData.h"
#include "ISSystem.h"
#include "ISConstants.h"
#include "ISQueryModelHelper.h"
#include "ISLogger.h"
//-----------------------------------------------------------------------------
ISQueryModel::ISQueryModel(PMetaClassTable *meta_table, ISNamespace::QueryModelType model_type, QObject *parent)
	: QObject(parent),
	MetaTable(meta_table),
	ModelType(model_type),
	ClassAlias(meta_table->Alias),
	ClassFilter(meta_table->ClassFilter),
	Limit(0),
	Offset(0),
	VisibleIsDeleted(false),
	PeriodType(ISNamespace::PT_UnknownDate),
	OrderSort(Qt::AscendingOrder)
{
	CreateQuerySelectSystemFields();
	CreateQuerySelectFields();
	CreateQuerySelectIsDeleted();

	QuerySelectText = "SELECT \n" + ClassAlias + SYMBOL_POINT + ClassAlias + '_' + MetaTable->GetFieldID()->Name.toLower() + " AS \"" + MetaTable->GetFieldID()->Name + "\", \n";
	QuerySelectFrom = "FROM " + MetaTable->Name.toLower() + SYMBOL_SPACE + ClassAlias + " \n";
	QueryWhereText = "WHERE \n";
	QueryWhereText += QuerySelectIsDeleted;
	OrderFieldDefault = ClassAlias + SYMBOL_POINT + ClassAlias + "_id";
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

	//���� ����� ������ �����������
	if (PeriodRange.IsValid())
	{
		QString PeriodString = QString("AND " + ClassAlias + SYMBOL_POINT + ClassAlias + "_%1 BETWEEN '%2' AND '%3' \n");
		switch (PeriodType)
		{
		case ISNamespace::PT_CreationDate: PeriodString = PeriodString.arg("creationdate"); break;
		case ISNamespace::PT_UpdationDate: PeriodString = PeriodString.arg("updationdate"); break;
		}
		SqlText += PeriodString.arg(QDateTime(PeriodRange.BeginValue.toDate(), QTime(0, 0)).toString(DATE_TIME_FORMAT_V7)).arg(QDateTime(PeriodRange.EndValue.toDate(), QTime(23, 59, 59)).toString(DATE_TIME_FORMAT_V7));
	}

	//���� ��� ������� ���������� ������
	if (ClassFilter.length())
	{
		SqlText += "AND " + ClassFilter + " \n";
	}

	//������ ������
	if (SearchFilter.length())
	{
		SqlText += "AND " + SearchFilter + " \n";
	}

	//���������� ����������
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
	QString FieldQueryText = MetaField->QueryText;
	if (FieldQueryText.length()) //���� ����������� ���� �������� ����������� - ����������� �� ������� ����
	{
		OrderField = '(' + FieldQueryText + ')';
	}
	else
	{
		if (MetaField->Foreign)
		{
			OrderField = ForeignFields[FieldName];
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
	for (int i = 1, c = MetaTable->SystemFields.count(); i < c; ++i) //����� ��������� ����� � ��������� �� � ������
	{
		PMetaClassField *SystemField = MetaTable->SystemFields[i];
		if (!MetaTable->SystemFieldsVisible.contains(SystemField))
		{
			if (SystemField->HideFromList)
			{
				continue;
			}
		}
		QuerySelectSystemFields += ClassAlias + SYMBOL_POINT + ClassAlias + '_' + SystemField->Name.toLower() + " AS \"" + SystemField->Name + "\", \n";
	}
}
//-----------------------------------------------------------------------------
void ISQueryModel::CreateQuerySelectFields()
{
	for (int i = 0, c = MetaTable->Fields.count(); i < c; ++i)
	{
		PMetaClassField *Field = MetaTable->Fields[i];

		if (ModelType == ISNamespace::QMT_Object)
		{
			if (Field->HideFromObject)
			{
				continue;
			}
		}
		else if (ModelType == ISNamespace::QMT_List)
		{
			if (Field->HideFromList)
			{
				continue;
			}
		}

		if (Field->Foreign) //���� �� ���� ���������� ������� ����
		{
			PMetaClassTable *MetaTableForeign = ISMetaData::GetInstanse().GetMetaTable(Field->Foreign->ForeignClass);
			QuerySelectLeftJoin += "LEFT JOIN " + MetaTableForeign->Name.toLower() + SYMBOL_SPACE + ISQueryModelHelper::GetAliasForLeftJoinTable(MetaTableForeign->Alias, i) + " ON " + ClassAlias + SYMBOL_POINT + ClassAlias + '_' + Field->Name.toLower() + " = " + ISQueryModelHelper::GetAliasForLeftJoinTable(MetaTableForeign->Alias, i) + SYMBOL_POINT + MetaTableForeign->Alias + '_' + Field->Foreign->ForeignField.toLower() + " \n";
			
			QString Temp = ISQueryModelHelper::GetForeignViewNameField(MetaTableForeign->Alias, Field->Foreign, i).toLower();
			ForeignFields.emplace(Field->Name, Temp);

			QuerySelectFields += Temp + " AS \"" + Field->Name + "\", \n";
		}
		else
		{
			if (!Field->QueryText.isEmpty())
			{
				QuerySelectFields += '(' + Field->QueryText + ") AS \"" + Field->Name + "\", \n";
			}
			else
			{
				QuerySelectFields += ClassAlias + SYMBOL_POINT + ClassAlias + '_' + Field->Name.toLower() + " AS \"" + Field->Name + "\", \n";
			}
		}
	}

	ISSystem::RemoveLastSymbolFromString(QuerySelectFields, 3);
	QuerySelectFields += " \n";
}
//-----------------------------------------------------------------------------
void ISQueryModel::CreateQuerySelectIsDeleted()
{
	QuerySelectIsDeleted = ClassAlias + SYMBOL_POINT + ClassAlias + "_isdeleted";
}
//-----------------------------------------------------------------------------
void ISQueryModel::CheckQuery(const QString &QueryText)
{
	ISQuery qQuery;
	if (qQuery.Prepare(QueryText))
	{
		ISLOGGER_DEBUG("Prepare Query - OK");
	}
	else
	{
		ISLOGGER_ERROR("Prepare Query - ERROR");
	}
}
//-----------------------------------------------------------------------------
