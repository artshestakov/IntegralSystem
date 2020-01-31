#include "StdAfx.h"
#include "ISSortingMetaTable.h"
#include "ISDefines.h"
//-----------------------------------------------------------------------------
ISSortingMetaTable::ISSortingMetaTable()
	: ModificationFlag(false)
{
	
}
//-----------------------------------------------------------------------------
ISSortingMetaTable::~ISSortingMetaTable()
{

}
//-----------------------------------------------------------------------------
void ISSortingMetaTable::SetTableName(const QString &table_name)
{
	TableName = table_name;
}
//-----------------------------------------------------------------------------
QString ISSortingMetaTable::GetTableName() const
{
	return TableName;
}
//-----------------------------------------------------------------------------
void ISSortingMetaTable::SetFieldName(const QString &field_name)
{
	FieldName = field_name;
}
//-----------------------------------------------------------------------------
QString ISSortingMetaTable::GetFieldName() const
{
	return FieldName;
}
//-----------------------------------------------------------------------------
void ISSortingMetaTable::SetSortingType(Qt::SortOrder sorting)
{
	Sorting = sorting;
}
//-----------------------------------------------------------------------------
Qt::SortOrder ISSortingMetaTable::GetSortingType() const
{
	return Sorting;
}
//-----------------------------------------------------------------------------
void ISSortingMetaTable::SetModificationFlag(bool modification_flag)
{
	ModificationFlag = modification_flag;
}
//-----------------------------------------------------------------------------
bool ISSortingMetaTable::GetModificationFlag() const
{
	return ModificationFlag;
}
//-----------------------------------------------------------------------------
