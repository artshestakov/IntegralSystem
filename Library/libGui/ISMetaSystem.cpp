#include "ISMetaSystem.h"
#include "EXDefines.h"
//-----------------------------------------------------------------------------
ISMetaSystem::ISMetaSystem()
	: IsSystem(false),
	ID(0),
	OrderID(0)
{
	
}
//-----------------------------------------------------------------------------
ISMetaSystem::~ISMetaSystem()
{
	while (!SubSystems.isEmpty())
	{
		delete SubSystems.takeLast();
	}
}
//-----------------------------------------------------------------------------
void ISMetaSystem::SetIsSystem(bool is_system)
{
	IsSystem = is_system;
}
//-----------------------------------------------------------------------------
bool ISMetaSystem::GetIsSystem() const
{
	return IsSystem;
}
//-----------------------------------------------------------------------------
void ISMetaSystem::SetID(int id)
{
	ID = id;
}
//-----------------------------------------------------------------------------
int ISMetaSystem::GetID() const
{
	return ID;
}
//-----------------------------------------------------------------------------
void ISMetaSystem::SetUID(const ISUuid &uid)
{
	UID = uid;
}
//-----------------------------------------------------------------------------
ISUuid ISMetaSystem::GetUID() const
{
	return UID;
}
//-----------------------------------------------------------------------------
void ISMetaSystem::SetLocalName(const QString &local_name)
{
	LocalName = local_name;
}
//-----------------------------------------------------------------------------
QString ISMetaSystem::GetLocalName() const
{
	return LocalName;
}
//-----------------------------------------------------------------------------
void ISMetaSystem::SetOrderID(int order_id)
{
	OrderID = order_id;
}
//-----------------------------------------------------------------------------
int ISMetaSystem::GetOrderID() const
{
	return OrderID;
}
//-----------------------------------------------------------------------------
void ISMetaSystem::SetIconName(const QString &icon_name)
{
	IconName = icon_name;
}
//-----------------------------------------------------------------------------
QString ISMetaSystem::GetIconName() const
{
	return IconName;
}
//-----------------------------------------------------------------------------
void ISMetaSystem::SetHint(const QString &hint)
{
	Hint = hint;
}
//-----------------------------------------------------------------------------
QString ISMetaSystem::GetHint() const
{
	return Hint;
}
//-----------------------------------------------------------------------------
void ISMetaSystem::AppendSubSystem(ISMetaSubSystem *MetaSubSystem)
{
	SubSystems.append(MetaSubSystem);
}
//-----------------------------------------------------------------------------
QVector<ISMetaSubSystem*> ISMetaSystem::GetSubSystems()
{
	return SubSystems;
}
//-----------------------------------------------------------------------------
