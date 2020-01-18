#include "PMetaClassEscort.h"
//-----------------------------------------------------------------------------
PMetaClassEscort::PMetaClassEscort(QObject *parent) : PMetaClass(parent)
{
	
}
//-----------------------------------------------------------------------------
PMetaClassEscort::~PMetaClassEscort()
{

}
//-----------------------------------------------------------------------------
void PMetaClassEscort::SetLocalName(const QString &local_name)
{
	LocalName = local_name;
}
//-----------------------------------------------------------------------------
QString PMetaClassEscort::GetLocalName() const
{
	return LocalName;
}
//-----------------------------------------------------------------------------
void PMetaClassEscort::SetTableName(const QString &table_name)
{
	TableName = table_name;
}
//-----------------------------------------------------------------------------
QString PMetaClassEscort::GetTableName() const
{
	return TableName;
}
//-----------------------------------------------------------------------------
void PMetaClassEscort::SetClassName(const QString &class_name)
{
	ClassName = class_name;
}
//-----------------------------------------------------------------------------
QString PMetaClassEscort::GetClassName() const
{
	return ClassName;
}
//-----------------------------------------------------------------------------
void PMetaClassEscort::SetClassFilter(const QString &class_filter)
{
	ClassFilter = class_filter;
}
//-----------------------------------------------------------------------------
QString PMetaClassEscort::GetClassFilter() const
{
	return ClassFilter;
}
//-----------------------------------------------------------------------------
