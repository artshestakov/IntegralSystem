#include "ISInterfaceMetaForm.h"
//-----------------------------------------------------------------------------
ISInterfaceMetaForm::ISInterfaceMetaForm(QWidget *parent)
	: ISInterfaceForm(parent),
	ParentObjectID(0)
{
	
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
QString ISInterfaceMetaForm::GetParentFilterField() const
{
	return ParentFilterField;
}
//-----------------------------------------------------------------------------
void ISInterfaceMetaForm::SetParentFilterField(const QString &parent_filter_field)
{
	ParentFilterField = parent_filter_field;
}
//-----------------------------------------------------------------------------
