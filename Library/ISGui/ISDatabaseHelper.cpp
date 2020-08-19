#include "ISDatabaseHelper.h"
#include "ISLocalization.h"
#include "ISQuery.h"
#include "ISAssert.h"
#include "ISConstants.h"
//-----------------------------------------------------------------------------
void ISDatabaseHelper::CheckValue(QVariant &Value)
{
	QVariant::Type ValueType = Value.type();
	if (ValueType == QVariant::Bool)
	{
		if (Value.toBool())
		{
			Value = LANG("Yes");
		}
		else
		{
			Value = LANG("No");
		}
	}
	else if (ValueType == QVariant::Date)
	{
		if (!Value.isValid())
		{
			Value = QVariant();
		}
		else
		{
			Value = Value.toDate().toString(FORMAT_DATE_V1);
		}
	}
	else if (ValueType == QVariant::DateTime)
	{
		if (!Value.toDateTime().isValid())
		{
			Value = QVariant();
		}
		else
		{
			Value = Value.toDateTime().toString(FORMAT_DATE_TIME_V3);
		}
	}
	else if (!Value.isValid())
	{
		Value.clear();
	}
}
//-----------------------------------------------------------------------------
QVariant ISDatabaseHelper::GetObjectIDToList(PMetaTable *MetaTable, PMetaField *MetaField, int ObjectID)
{
	QString FieldName = MetaTable->Alias + '_' + MetaField->Name;
	ISQuery qSelect(QString("SELECT %1 FROM %2 WHERE %3_id = %4").arg(FieldName).arg(MetaTable->Name).arg(MetaTable->Alias).arg(ObjectID));
	if (qSelect.ExecuteFirst())
	{
		return qSelect.ReadColumn(FieldName);
	}
	IS_ASSERT(false, "MetaTable: " + MetaTable->Name + ". MetaField: " + MetaField->Name + ". ObjectID = " + QString::number(ObjectID));
	return QVariant();
}
//-----------------------------------------------------------------------------
int ISDatabaseHelper::GetCountRows(const QString &TableName, const QString &Alias)
{
	ISQuery qSelectCount(QString("SELECT COUNT(*) FROM %1 WHERE NOT %2_isdeleted").arg(TableName).arg(Alias));
	return qSelectCount.ExecuteFirst() ? qSelectCount.ReadColumn("count").toInt() : -1;
}
//-----------------------------------------------------------------------------
