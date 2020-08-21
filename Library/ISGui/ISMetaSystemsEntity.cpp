#include "ISMetaSystemsEntity.h"
#include "ISQuery.h"
#include "ISMetaUser.h"
#include "ISUserRoleEntity.h"
//-----------------------------------------------------------------------------
static QString QS_SYSTEMS = PREPARE_QUERY("SELECT "
										  "stms_issystem, stms_id, stms_uid, stms_localname, stms_orderid, stms_icon, stms_hint, "
										  "sbsm_id, sbsm_uid, sbsm_localname, sbsm_orderid, sbsm_icon, sbsm_classname, sbsm_tablename, sbsm_hint "
										  "FROM _subsystems "
										  "LEFT JOIN _systems ON stms_uid = sbsm_system "
										  "WHERE NOT sbsm_isdeleted "
										  "ORDER BY stms_orderid, sbsm_orderid");
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
ISMetaSystemsEntity& ISMetaSystemsEntity::GetInstance()
{
	static ISMetaSystemsEntity MetaSystemsEntity;
	return MetaSystemsEntity;
}
//-----------------------------------------------------------------------------
void ISMetaSystemsEntity::Initialize()
{
	ISQuery qSelect(QS_SYSTEMS);
	if (qSelect.Execute())
	{
		while (qSelect.Next())
		{
			ISUuid SystemUID = qSelect.ReadColumn("stms_uid"), SubSystemUID = qSelect.ReadColumn("sbsm_uid");
			if (!ISMetaUser::Instance().UserData->System) //≈сли текущий пользователь не системный
			{
				if (!ISMetaUser::Instance().UserData->GroupFullAccess) //≈сли у группы пользовател€ нет полного доступа - провер€ть доступ к подсистемам
				{
					if (!ISUserRoleEntity::Instance().CheckAccessSubSystem(SubSystemUID)) //≈сли доступа к подсистеме нет - перезодить на следующую итерацию цикла
					{
						continue;
					}
				}
			}

			ISMetaSystem *MetaSystem = CheckExistSystem(SystemUID);
			if (!MetaSystem)
			{
				MetaSystem = new ISMetaSystem();
				MetaSystem->IsSystem = qSelect.ReadColumn("stms_issystem").toBool();
				MetaSystem->ID = qSelect.ReadColumn("stms_id").toInt();
				MetaSystem->UID = SystemUID;
				MetaSystem->LocalName = qSelect.ReadColumn("stms_localname").toString();
				MetaSystem->OrderID = qSelect.ReadColumn("stms_orderid").toInt();
				MetaSystem->IconName = qSelect.ReadColumn("stms_icon").toString();
				MetaSystem->Hint = qSelect.ReadColumn("stms_hint").toString();
				Systems.push_back(MetaSystem);
			}

			ISMetaSubSystem *MetaSubSystem = new ISMetaSubSystem();
			MetaSubSystem->ID = qSelect.ReadColumn("sbsm_id").toInt();
			MetaSubSystem->UID = SubSystemUID;
			MetaSubSystem->LocalName = qSelect.ReadColumn("sbsm_localname").toString();
			MetaSubSystem->OrderID = qSelect.ReadColumn("sbsm_orderid").toInt();
			MetaSubSystem->IconName = qSelect.ReadColumn("sbsm_icon").toString();
			MetaSubSystem->ClassName = qSelect.ReadColumn("sbsm_classname").toString();
			MetaSubSystem->TableName = qSelect.ReadColumn("sbsm_tablename").toString();
			MetaSubSystem->Hint = qSelect.ReadColumn("sbsm_hint").toString();
			MetaSubSystem->SystemUID = SystemUID;
			MetaSystem->SubSystems.push_back(MetaSubSystem);
		}
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
ISMetaSystem* ISMetaSystemsEntity::CheckExistSystem(const ISUuid &SystemUID)
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
