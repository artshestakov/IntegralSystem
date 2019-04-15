#include "StdAfx.h"
#include "ISInterfaceMetaForm.h"
//-----------------------------------------------------------------------------
ISInterfaceMetaForm::ISInterfaceMetaForm(QWidget *parent) : ISInterfaceForm(parent)
{
	ParentObjectID = 0;
}
//-----------------------------------------------------------------------------
ISInterfaceMetaForm::~ISInterfaceMetaForm()
{

}
//-----------------------------------------------------------------------------
QString ISInterfaceMetaForm::GetParentTableName() const
{
	return ParentTableName;
}
//-----------------------------------------------------------------------------
void ISInterfaceMetaForm::SetParentTableName(const QString &table_name)
{
	ParentTableName = table_name;
}
//-----------------------------------------------------------------------------
int ISInterfaceMetaForm::GetParentObjectID() const
{
	return ParentObjectID;
}
//-----------------------------------------------------------------------------
void ISInterfaceMetaForm::SetParentObjectID(int parent_object_id)
{
	ParentObjectID = parent_object_id;
}
//-----------------------------------------------------------------------------
void ISInterfaceMetaForm::SetUID(const QString &sub_system_uid)
{
	UID = sub_system_uid;
}
//-----------------------------------------------------------------------------
ISUuid ISInterfaceMetaForm::GetUID() const
{
	return UID;
}
//-----------------------------------------------------------------------------
