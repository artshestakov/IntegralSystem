#include "StdAfx.h"
#include "ISMetaSubSystem.h"
#include "ISDefines.h"
//-----------------------------------------------------------------------------
ISMetaSubSystem::ISMetaSubSystem()
	: ID(0),
	OrderID(0),
	SystemID(0)
{
	
}
//-----------------------------------------------------------------------------
ISMetaSubSystem::~ISMetaSubSystem()
{

}
//-----------------------------------------------------------------------------
void ISMetaSubSystem::SetID(int id)
{
	ID = id;
}
//-----------------------------------------------------------------------------
int ISMetaSubSystem::GetID() const
{
	return ID;
}
//-----------------------------------------------------------------------------
void ISMetaSubSystem::SetUID(const ISUuid &uid)
{
	UID = uid;
}
//-----------------------------------------------------------------------------
ISUuid ISMetaSubSystem::GetUID() const
{
	return UID;
}
//-----------------------------------------------------------------------------
void ISMetaSubSystem::SetLocalName(const QString &local_name)
{
	LocalName = local_name;
}
//-----------------------------------------------------------------------------
QString ISMetaSubSystem::GetLocalName() const
{
	return LocalName;
}
//-----------------------------------------------------------------------------
void ISMetaSubSystem::SetOrderID(int order_id)
{
	OrderID = order_id;
}
//-----------------------------------------------------------------------------
int ISMetaSubSystem::GetOrderID() const
{
	return OrderID;
}
//-----------------------------------------------------------------------------
void ISMetaSubSystem::SetIconName(const QString &icon_name)
{
	IconName = icon_name;
}
//-----------------------------------------------------------------------------
QString ISMetaSubSystem::GetIconName() const
{
	return IconName;
}
//-----------------------------------------------------------------------------
void ISMetaSubSystem::SetTableName(const QString &table_name)
{
	TableName = table_name;
}
//-----------------------------------------------------------------------------
QString ISMetaSubSystem::GetTableName() const
{
	return TableName;
}
//-----------------------------------------------------------------------------
void ISMetaSubSystem::SetClassName(const QString &class_name)
{
	ClassName = class_name;
}
//-----------------------------------------------------------------------------
QString ISMetaSubSystem::GetClassName() const
{
	return ClassName;
}
//-----------------------------------------------------------------------------
void ISMetaSubSystem::SetHint(const QString &hint)
{
	Hint = hint;
}
//-----------------------------------------------------------------------------
QString ISMetaSubSystem::GetHint() const
{
	return Hint;
}
//-----------------------------------------------------------------------------
void ISMetaSubSystem::SetSystemUID(const QString &system_uid)
{
	SystemUID = system_uid;
}
//-----------------------------------------------------------------------------
QString ISMetaSubSystem::GetSystemUID() const
{
	return SystemUID;
}
//-----------------------------------------------------------------------------
void ISMetaSubSystem::SetSystemID(int system_id)
{
	SystemID = system_id;
}
//-----------------------------------------------------------------------------
int ISMetaSubSystem::GetSystemID() const
{
	return SystemID;
}
//-----------------------------------------------------------------------------
void ISMetaSubSystem::SetSystemLocalName(const QString &system_local_name)
{
	SystemLocalName = system_local_name;
}
//-----------------------------------------------------------------------------
QString ISMetaSubSystem::GetSystemLocalName() const
{
	return SystemLocalName;
}
//-----------------------------------------------------------------------------
