#include "ISMetaSystemsEntity.h"
#include "ISQuery.h"
#include "ISMetaUser.h"
#include "ISUserRoleEntity.h"
//-----------------------------------------------------------------------------
static QString QS_SYSTEMS = PREPARE_QUERY("SELECT "
										  "stms_issystem, stms_uid, stms_localname, stms_icon, stms_hint, "
										  "sbsm_uid, sbsm_localname, sbsm_icon, sbsm_classname, sbsm_tablename, sbsm_hint "
										  "FROM _subsystems "
										  "LEFT JOIN _systems ON stms_uid = sbsm_system "
										  "WHERE NOT sbsm_isdeleted "
										  "ORDER BY stms_orderid, sbsm_orderid");
//-----------------------------------------------------------------------------
ISMetaSystemsEntity::ISMetaSystemsEntity()
	: ErrorString(NO_ERROR_STRING)
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
QString ISMetaSystemsEntity::GetErrorString() const
{
	return ErrorString;
}
//-----------------------------------------------------------------------------
void ISMetaSystemsEntity::Initialize(const QVariantList &VariantList)
{
	for (const QVariant &System : VariantList) //Обходим системы
	{
		QVariantMap SystemMap = System.toMap();
		ISMetaSystem *MetaSystem = new ISMetaSystem();
		MetaSystem->IsSystem = SystemMap["IsSystem"].toBool();
		MetaSystem->UID = SystemMap["UID"];
		MetaSystem->LocalName = SystemMap["Local"].toString();
		MetaSystem->IconName = SystemMap["Icon"].toString();
		MetaSystem->Hint = SystemMap["Hint"].toString();

		QVariantList SubSystems = SystemMap["SubSystems"].toList();
		for (const QVariant &SubSystem : SubSystems) //Обходим подсистемы
		{
			QVariantMap SubSystemMap = SubSystem.toMap();
			ISMetaSubSystem *MetaSubSystem = new ISMetaSubSystem();
			MetaSubSystem->UID = SubSystemMap["UID"];
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
bool ISMetaSystemsEntity::Initialize()
{
	ISQuery qSelect(QS_SYSTEMS);
	bool Result = qSelect.Execute();
	if (Result)
	{
		while (qSelect.Next())
		{
			ISUuid SystemUID = qSelect.ReadColumn("stms_uid"), SubSystemUID = qSelect.ReadColumn("sbsm_uid");
			if (!ISMetaUser::Instance().UserData.System) //Если текущий пользователь не системный
			{
				if (!ISMetaUser::Instance().UserData.GroupFullAccess) //Если у группы пользователя нет полного доступа - проверять доступ к подсистемам
				{
					if (!ISUserRoleEntity::Instance().CheckAccessSubSystem(SubSystemUID)) //Если доступа к подсистеме нет - переходить на следующую итерацию цикла
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
				MetaSystem->UID = SystemUID;
				MetaSystem->LocalName = qSelect.ReadColumn("stms_localname").toString();
				MetaSystem->IconName = qSelect.ReadColumn("stms_icon").toString();
				MetaSystem->Hint = qSelect.ReadColumn("stms_hint").toString();
				Systems.emplace_back(MetaSystem);
			}

			ISMetaSubSystem *MetaSubSystem = new ISMetaSubSystem();
			MetaSubSystem->UID = SubSystemUID;
			MetaSubSystem->LocalName = qSelect.ReadColumn("sbsm_localname").toString();
			MetaSubSystem->IconName = qSelect.ReadColumn("sbsm_icon").toString();
			MetaSubSystem->ClassName = qSelect.ReadColumn("sbsm_classname").toString();
			MetaSubSystem->TableName = qSelect.ReadColumn("sbsm_tablename").toString();
			MetaSubSystem->Hint = qSelect.ReadColumn("sbsm_hint").toString();
			MetaSubSystem->SystemUID = SystemUID;
			MetaSystem->SubSystems.emplace_back(MetaSubSystem);
		}
	}
	else
	{
		ErrorString = qSelect.GetErrorString();
	}
	return Result;
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
