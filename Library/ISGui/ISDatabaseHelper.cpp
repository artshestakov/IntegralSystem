#include "ISDatabaseHelper.h"
#include "ISQuery.h"
#include "ISAssert.h"
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
