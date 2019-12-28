#include "StdAfx.h"
#include "PMetaClassIndex.h"
#include "ISDefines.h"
//-----------------------------------------------------------------------------
PMetaClassIndex::PMetaClassIndex(QObject *parent) : QObject(parent)
{
	Unique = false;
}
//-----------------------------------------------------------------------------
PMetaClassIndex::PMetaClassIndex(bool unique, const QString &alias, const QString &table_name, const QString &field_name, QObject *parent) : QObject(parent)
{
	Unique = unique;
	Alias = alias;
	TableName = table_name;
	FieldName = field_name;
}
//-----------------------------------------------------------------------------
PMetaClassIndex::~PMetaClassIndex()
{

}
//-----------------------------------------------------------------------------
void PMetaClassIndex::SetUnique(bool unique)
{
	Unique = unique;
}
//-----------------------------------------------------------------------------
bool PMetaClassIndex::GetUnique() const
{
	return Unique;
}
//-----------------------------------------------------------------------------
void PMetaClassIndex::SetAlias(const QString &alias)
{
	Alias = alias;
}
//-----------------------------------------------------------------------------
QString PMetaClassIndex::GetAlias() const
{
	return Alias;
}
//-----------------------------------------------------------------------------
void PMetaClassIndex::SetTableName(const QString &table_name)
{
	TableName = table_name;
}
//-----------------------------------------------------------------------------
QString PMetaClassIndex::GetTableName() const
{
	return TableName;
}
//-----------------------------------------------------------------------------
void PMetaClassIndex::SetFieldName(const QString &field_name)
{
	FieldName = field_name;
}
//-----------------------------------------------------------------------------
QString PMetaClassIndex::GetFieldName() const
{
	return FieldName;
}
//-----------------------------------------------------------------------------
void PMetaClassIndex::AddField(const QString &Field)
{
	Fields.append(Field);
}
//-----------------------------------------------------------------------------
QVectorString PMetaClassIndex::GetFields()
{
	return Fields;
}
//-----------------------------------------------------------------------------
