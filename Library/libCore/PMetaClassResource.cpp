#include "PMetaClassResource.h"
#include "ISAssert.h"
//-----------------------------------------------------------------------------
PMetaClassResource::PMetaClassResource()
{
	
}
//-----------------------------------------------------------------------------
PMetaClassResource::~PMetaClassResource()
{

}
//-----------------------------------------------------------------------------
void PMetaClassResource::AddField(const QString &FieldName, const QString &Value)
{
	if (FieldName.toLower() == "uid")
	{
		IS_ASSERT(!UID.length(), "Already exist uid " + UID);
		SetUID(Value);
	}
	else
	{
		Parameters.insert(FieldName, Value);
	}
}
//-----------------------------------------------------------------------------
QString PMetaClassResource::GetParameterValue(const QString &ParameterName) const
{
	QString Value = Parameters.value(ParameterName);
	return Value;
}
//-----------------------------------------------------------------------------
QStringMap PMetaClassResource::GetParameters()
{
	return Parameters;
}
//-----------------------------------------------------------------------------
void PMetaClassResource::SetTableName(const QString &table_name)
{
	TableName = table_name;
}
//-----------------------------------------------------------------------------
QString PMetaClassResource::GetTableName() const
{
	return TableName;
}
//-----------------------------------------------------------------------------
void PMetaClassResource::SetUID(const ISUuid &uid)
{
	UID = uid;
}
//-----------------------------------------------------------------------------
ISUuid PMetaClassResource::GetUID() const
{
	return UID;
}
//-----------------------------------------------------------------------------
