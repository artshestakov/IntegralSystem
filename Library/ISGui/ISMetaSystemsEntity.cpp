#include "ISMetaSystemsEntity.h"
#include "ISUserRoleEntity.h"
#include "ISBuffer.h"
//-----------------------------------------------------------------------------
ISMetaSystemsEntity::ISMetaSystemsEntity()
{
	
}
//-----------------------------------------------------------------------------
ISMetaSystemsEntity::~ISMetaSystemsEntity()
{
	while (!Systems.empty())
	{
		delete ISAlgorithm::VectorTakeBack(Systems);
	}
}
//-----------------------------------------------------------------------------
ISMetaSystemsEntity& ISMetaSystemsEntity::Instance()
{
	static ISMetaSystemsEntity MetaSystemsEntity;
	return MetaSystemsEntity;
}
//-----------------------------------------------------------------------------
void ISMetaSystemsEntity::Initialize(const QVariantList &VariantList)
{
	for (const QVariant &System : VariantList) //Обходим системы
	{
		QVariantMap SystemMap = System.toMap();
		ISMetaSystem *MetaSystem = new ISMetaSystem();
		MetaSystem->IsSystem = SystemMap["IsSystem"].toBool();
		MetaSystem->UID = SystemMap["UID"].toString();
		MetaSystem->LocalName = SystemMap["Local"].toString();
		MetaSystem->IconName = SystemMap["Icon"].toString();
		MetaSystem->Hint = SystemMap["Hint"].toString();

		QVariantList SubSystems = SystemMap["SubSystems"].toList();
		for (const QVariant &SubSystem : SubSystems) //Обходим подсистемы
		{
			QVariantMap SubSystemMap = SubSystem.toMap();
			ISMetaSubSystem *MetaSubSystem = new ISMetaSubSystem();
			MetaSubSystem->UID = SubSystemMap["UID"].toString();
			MetaSubSystem->LocalName = SubSystemMap["Local"].toString();
			MetaSubSystem->IconName = SubSystemMap["Icon"].toString();
			MetaSubSystem->ClassName = SubSystemMap["Class"].toString();
			MetaSubSystem->TableName = SubSystemMap["Table"].toString();
			MetaSubSystem->Hint = SubSystemMap["Hint"].toString();
			MetaSubSystem->SystemUID = MetaSystem->UID;
			MetaSystem->SubSystems.emplace_back(MetaSubSystem);
		}
		Systems.emplace_back(MetaSystem);
	}
}
//-----------------------------------------------------------------------------
std::vector<ISMetaSystem*> ISMetaSystemsEntity::GetSystems()
{
	return Systems;
}
//-----------------------------------------------------------------------------
ISMetaSystem* ISMetaSystemsEntity::GetSystem(const QString &SystemUID)
{
	ISMetaSystem *MetaSystem = CheckExistSystem(SystemUID);
	if (MetaSystem)
	{
		return MetaSystem;
	}
	return nullptr;
}
//-----------------------------------------------------------------------------
ISMetaSubSystem* ISMetaSystemsEntity::GetSubSystem(const QString &SubSystemUID)
{
	for (ISMetaSystem *MetaSystem : Systems)
	{
		for (ISMetaSubSystem *MetaSubSystem : MetaSystem->SubSystems)
		{
			if (MetaSubSystem->UID == SubSystemUID)
			{
				return MetaSubSystem;
			}
		}
	}
	return nullptr;
}
//-----------------------------------------------------------------------------
ISMetaSystem* ISMetaSystemsEntity::CheckExistSystem(const QString &SystemUID)
{
	for (ISMetaSystem *MetaSystem : Systems)
	{
		if (MetaSystem->UID == SystemUID)
		{
			return MetaSystem;
		}
	}
	return nullptr;
}
//-----------------------------------------------------------------------------
