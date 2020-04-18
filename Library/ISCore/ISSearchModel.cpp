#include "ISSearchModel.h"
#include "ISSystem.h"
#include "ISRandom.h"
//-----------------------------------------------------------------------------
ISSearchModel::ISSearchModel()
{

}
//-----------------------------------------------------------------------------
ISSearchModel::~ISSearchModel()
{

}
//-----------------------------------------------------------------------------
void ISSearchModel::CreateSearchString(QString &SearchString, QVariantMap &Conditions) const
{
	for (int i = 0; i < Vector.count(); ++i)
	{
		StructModelItem ModelItem = Vector.at(i);
		if (ModelItem.Values.size() > 1) //����� ���������� �������� �� ������ ����
		{
			for (const QVariant &Value : ModelItem.Values)
			{
				QString Condition = ':' + ModelItem.FieldName + '_' + ISRandom::String();

				if (ModelItem.FieldType == ISNamespace::FT_String)
				{
					SearchString += CreateSubString(ModelItem.Operator, "lower(" + ModelItem.FieldName + ')', Condition, false);
				}
				else
				{
					SearchString += CreateSubString(ModelItem.Operator, ModelItem.FieldName, Condition, false);
				}
				
				Conditions.insert(Condition, Value);
			}
			SearchString.chop(4);
		}
		else //����� ������ ��������
		{
			QString Condition = ':' + ModelItem.FieldName + '_' + ISRandom::String();
			if (ModelItem.FieldType == ISNamespace::FT_String)
			{
				SearchString += CreateSubString(ModelItem.Operator, "lower(" + ModelItem.FieldName + ')', Condition, true);
			}
			else
			{
				SearchString += CreateSubString(ModelItem.Operator, ModelItem.FieldName, Condition, true);
			}
			Conditions.insert(Condition, ModelItem.Values.front());
		}

		SearchString += " \nAND ";
	}
	SearchString.chop(5);
}
//-----------------------------------------------------------------------------
void ISSearchModel::AddField(const QString &FieldName, const QVariant &Value, ISNamespace::SearchOperatorType Operator, ISNamespace::FieldType FieldType)
{
	if (Vector.count())
	{
		for (int i = 0; i < Vector.count(); ++i)
		{
			StructModelItem ModelItem = Vector.at(i);

			if (Vector.at(i).FieldName == FieldName && Vector.at(i).Operator == Operator)
			{
				Vector[i].Values.emplace_back(Value); //������ ����������� ������ ���� �� ������� � ���������� �������
				return;
			}
		}
	}

	StructModelItem Item;
	Item.FieldName = FieldName;
	Item.Values.emplace_back(Value);
	Item.Operator = Operator;
	Item.FieldType = FieldType;
	Vector.append(Item);
}
//-----------------------------------------------------------------------------
QString ISSearchModel::CreateSubString(ISNamespace::SearchOperatorType Operator, const QString &FieldName, const QString &Condition, bool OneField) const
{
	QString ResultString;

	if (OneField)
	{
		switch (Operator)
		{
		case ISNamespace::SOT_Equally: ResultString = FieldName + " = " + Condition; break;
		case ISNamespace::SOT_NotEqually: ResultString = FieldName + " <> " + Condition; break;
		case ISNamespace::SOT_Begins: ResultString = FieldName + " LIKE " + Condition + " || '%'"; break;
		case ISNamespace::SOT_Ends: ResultString = FieldName + " LIKE '%' || " + Condition; break;
		case ISNamespace::SOT_Contains: ResultString = FieldName + " LIKE '%' || " + Condition + " || '%'"; break;
		case ISNamespace::SOT_More: ResultString = FieldName + " > " + Condition; break;
		case ISNamespace::SOT_Less: ResultString = FieldName + " < " + Condition; break;
		case ISNamespace::SOT_MoreOrEqual: ResultString = FieldName + " >= " + Condition; break;
		case ISNamespace::SOT_LessOrEqual: ResultString = FieldName + " <= " + Condition; break;
		}
	}
	else
	{
		switch (Operator)
		{
		case ISNamespace::SOT_Equally: ResultString = FieldName + " = " + Condition + " OR "; break;
		case ISNamespace::SOT_NotEqually: ResultString = FieldName + " <> " + Condition + " OR "; break;
		case ISNamespace::SOT_Begins: ResultString = FieldName + " LIKE " + Condition + " || '%' OR "; break;
		case ISNamespace::SOT_Ends: ResultString = FieldName + " LIKE '%' || " + Condition + " OR "; break;
		case ISNamespace::SOT_Contains: ResultString = FieldName + " LIKE '%' || " + Condition + " || '%' OR "; break;
		case ISNamespace::SOT_More: ResultString = FieldName + " > " + Condition + " OR "; break;
		case ISNamespace::SOT_Less: ResultString = FieldName + " < " + Condition + " OR "; break;
		case ISNamespace::SOT_MoreOrEqual: ResultString = FieldName + " >= " + Condition + " OR "; break;
		case ISNamespace::SOT_LessOrEqual: ResultString = FieldName + " <= " + Condition + " OR "; break;
		}
	}

	return ResultString;
}
//-----------------------------------------------------------------------------
