#include "ISMetaSystemsEntity.h"
#include "ISDefines.h"
#include "ISConstants.h"
#include "ISQuery.h"
#include "ISAssert.h"
#include "ISCountingTime.h"
#include "ISDebug.h"
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
	while (!Systems.isEmpty())
	{
		delete Systems.takeLast();
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
	ISCountingTime CountingTime;

	ISQuery qSelect(QS_SYSTEMS);
	if (qSelect.Execute())
	{
		while (qSelect.Next())
		{
			bool SystemIsSystem = qSelect.ReadColumn("stms_issystem").toBool();
			int SystemID = qSelect.ReadColumn("stms_id").toInt();
			ISUuid SystemUID = qSelect.ReadColumn("stms_uid");
			QString SystemLocalName = qSelect.ReadColumn("stms_localname").toString();
			int SystemOrderID = qSelect.ReadColumn("stms_orderid").toInt();
			QString SystemIcon = qSelect.ReadColumn("stms_icon").toString();
			QString SystemHint = qSelect.ReadColumn("stms_hint").toString();

			int SubSystemID = qSelect.ReadColumn("sbsm_id").toInt();
			ISUuid SubSystemUID = qSelect.ReadColumn("sbsm_uid");
			QString SubSystemLocalName = qSelect.ReadColumn("sbsm_localname").toString();
			int SubSystemOrderID = qSelect.ReadColumn("sbsm_orderid").toInt();
			QString SubSystemIcon = qSelect.ReadColumn("sbsm_icon").toString();
			QString SubSystemClassName = qSelect.ReadColumn("sbsm_classname").toString();
			QString SubSystemTableName = qSelect.ReadColumn("sbsm_tablename").toString();
			QString SubSystemHint = qSelect.ReadColumn("sbsm_hint").toString();

			if (!ISMetaUser::GetInstance().GetData()->System) //≈сли текущий пользователь не системный
			{
				if (!ISMetaUser::GetInstance().GetData()->GroupFullAccess) //≈сли у группы пользовател€ нет полного доступа - провер€ть доступ к подсистемам
				{
					if (!ISUserRoleEntity::GetInstance().CheckAccessSubSystem(SubSystemUID)) //≈сли доступа к подсистеме нет - перезодить на следующую итерацию цикла
					{
						continue;
					}
				}
			}

			ISMetaSystem *MetaSystem = CheckExistSystem(SystemUID);
			if (!MetaSystem)
			{
				MetaSystem = new ISMetaSystem();
				MetaSystem->SetIsSystem(SystemIsSystem);
				MetaSystem->SetID(SystemID);
				MetaSystem->SetUID(SystemUID);
				MetaSystem->SetLocalName(SystemLocalName);
				MetaSystem->SetOrderID(SystemOrderID);
				MetaSystem->SetIconName(SystemIcon);
				MetaSystem->SetHint(SystemHint);
				Systems.append(MetaSystem);
			}

			ISMetaSubSystem *MetaSubSystem = new ISMetaSubSystem();
			MetaSubSystem->SetID(SubSystemID);
			MetaSubSystem->SetUID(SubSystemUID);
			MetaSubSystem->SetLocalName(SubSystemLocalName);
			MetaSubSystem->SetOrderID(SubSystemOrderID);
			MetaSubSystem->SetIconName(SubSystemIcon);
			MetaSubSystem->SetClassName(SubSystemClassName);
			MetaSubSystem->SetTableName(SubSystemTableName);
			MetaSubSystem->SetHint(SubSystemHint);
			MetaSubSystem->SetSystemUID(SystemUID);
			MetaSystem->AppendSubSystem(MetaSubSystem);
		}
	}

	ISDebug::ShowDebugString(QString("Initialized MetaSystems %1 msec").arg(CountingTime.GetElapsed()));
}
//-----------------------------------------------------------------------------
QVector<ISMetaSystem*> ISMetaSystemsEntity::GetSystems()
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
		for (ISMetaSubSystem *MetaSubSystem : MetaSystem->GetSubSystems())
		{
			if (MetaSubSystem->GetUID() == SubSystemUID)
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
		if (MetaSystem->GetUID() == SystemUID)
		{
			return MetaSystem;
		}
	}
	return nullptr;
}
//-----------------------------------------------------------------------------
