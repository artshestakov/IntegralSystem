#include "ISDatabaseHelper.h"
#include "ISLocalization.h"
#include "EXDefines.h"
#include "ISQuery.h"
#include "ISAssert.h"
//-----------------------------------------------------------------------------
QVariant ISDatabaseHelper::CheckValue(QVariant &Value)
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
			Value = Value.toDate().toString(DATE_FORMAT_V1);
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
			Value = Value.toDateTime().toString(DATE_TIME_FORMAT_V3);
		}
	}
	else if (!Value.isValid())
	{
		Value.clear();
	}

	return Value;
}
//-----------------------------------------------------------------------------
QVariant ISDatabaseHelper::GetObjectIDToList(PMetaClassTable *MetaTable, PMetaClassField *MetaField, int ObjectID)
{
	QString SqlText;
	QString TableAlias = MetaTable->GetAlias();

	SqlText = "SELECT " + TableAlias + '_' + MetaField->GetName() + " FROM " + MetaTable->GetName() + " WHERE " + TableAlias + "_id = " + QString::number(ObjectID);

	ISQuery qSelect(SqlText);
	if (qSelect.ExecuteFirst())
	{
		QVariant ID = qSelect.ReadColumn(TableAlias + '_' + MetaField->GetName());
		return ID;
	}

	IS_ASSERT(false, "MetaTable: " + MetaTable->GetName() + ". MetaField: " + MetaField->GetName() + ". ObjectID = " + QString::number(ObjectID));
	return QVariant();
}
//-----------------------------------------------------------------------------
int ISDatabaseHelper::GetCountRows(const QString &TableName, const QString &Alias)
{
	ISQuery qSelectCount(QString("SELECT COUNT(*) FROM %1 WHERE NOT %2_isdeleted").arg(TableName).arg(Alias));
	return qSelectCount.ExecuteFirst() ? qSelectCount.ReadColumn("count").toInt() : -1;
}
//-----------------------------------------------------------------------------
