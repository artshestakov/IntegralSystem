#include "ISUserRoleEntity.h"
#include "ISQuery.h"
#include "ISMetaUser.h"
#include "ISMetaSystemsEntity.h"
#include "ISCountingTime.h"
#include "ISLogger.h"
//-----------------------------------------------------------------------------
static QString QS_GROUP_ACCESS_SUBSYSTEM_CHECK = PREPARE_QUERY("SELECT COUNT(*) "
															   "FROM _groupaccesssubsystem "
															   "WHERE gass_group = :GroupID "
															   "AND gass_subsystem = :SubSystemUID");
//-----------------------------------------------------------------------------
static QString QI_GROUP_ACCESS_SUBSYSTEM = PREPARE_QUERY("INSERT INTO _groupaccesssubsystem(gass_group, gass_subsystem) "
														 "VALUES(:GroupID, :SubSystemUID)");
//-----------------------------------------------------------------------------
static QString QD_GROUP_ACCESS_SUBSYSTEM = PREPARE_QUERY("DELETE FROM _groupaccesssubsystem "
														 "WHERE gass_group = :GroupID "
														 "AND gass_subsystem = :SubSystemUID");
//-----------------------------------------------------------------------------
static QString QS_GROUP_ACCESS_TABLE_CHECK = PREPARE_QUERY("SELECT COUNT(*) "
														   "FROM _groupaccesstable "
														   "WHERE gatb_group = :GroupID "
														   "AND gatb_table = :TableUID "
														   "AND gatb_accesstype = :AccessTypeUID");
//-----------------------------------------------------------------------------
static QString QI_GROUP_ACCESS_TABLE = PREPARE_QUERY("INSERT INTO _groupaccesstable(gatb_group, gatb_table, gatb_accesstype) "
													 "VALUES(:GroupID, :TableUID, :AccessTypeUID)");
//-----------------------------------------------------------------------------
static QString QD_GROUP_ACCESS_TABLE = PREPARE_QUERY("DELETE FROM _groupaccesstable "
													 "WHERE gatb_group = :GroupID "
													 "AND gatb_table = :TableUID "
													 "AND gatb_accesstype = :AccessTypeUID");
//-----------------------------------------------------------------------------
static QString QS_GROUP_ACCESS_SPECIAL_CHECK = PREPARE_QUERY("SELECT COUNT(*) "
															 "FROM _groupaccessspecial "
															 "WHERE gasp_group = :GroupID "
															 "AND gasp_specialaccess = :SpecialAccessUID");
//-----------------------------------------------------------------------------
static QString QI_GROUP_ACCESS_SPECIAL = PREPARE_QUERY("INSERT INTO _groupaccessspecial(gasp_group, gasp_specialaccess) "
													   "VALUES(:GroupID, :SpecialAccessUID)");
//-----------------------------------------------------------------------------
static QString QD_GROUP_ACCESS_SPECIAL = PREPARE_QUERY("DELETE FROM _groupaccessspecial "
													   "WHERE gasp_group = :GroupID "
													   "AND gasp_specialaccess = :SpecialAccessUID");
//-----------------------------------------------------------------------------
static QString QS_GROUP_ACCESS_SUBSYSTEM = PREPARE_QUERY("SELECT gass_subsystem FROM _groupaccesssubsystem WHERE gass_group = :GroupID");
//-----------------------------------------------------------------------------
static QString QS_GROUP_ACCESS_TABLE = PREPARE_QUERY("SELECT gatb_table, gatb_accesstype FROM _groupaccesstable WHERE gatb_group = :GroupID");
//-----------------------------------------------------------------------------
static QString QS_GROUP_ACCESS_SPECIAL = PREPARE_QUERY("SELECT gasp_specialaccess FROM _groupaccessspecial WHERE gasp_group = :GroupID");
//-----------------------------------------------------------------------------
ISUserRoleEntity::ISUserRoleEntity()
{
	
}
//-----------------------------------------------------------------------------
ISUserRoleEntity::~ISUserRoleEntity()
{

}
//-----------------------------------------------------------------------------
ISUserRoleEntity& ISUserRoleEntity::GetInstance()
{
	static ISUserRoleEntity UserRoleEntity;
	return UserRoleEntity;
}
//-----------------------------------------------------------------------------
bool ISUserRoleEntity::CheckExistSubSystemAccess(int GroupID, const ISUuid &SubSystemUID)
{
	ISQuery qSelect(QS_GROUP_ACCESS_SUBSYSTEM_CHECK);
	qSelect.BindValue(":GroupID", GroupID);
	qSelect.BindValue(":SubSystemUID", SubSystemUID);
	if (qSelect.ExecuteFirst())
	{
		if (qSelect.ReadColumn("count").toInt())
		{
			return true;
		}
	}

	return false;
}
//-----------------------------------------------------------------------------
void ISUserRoleEntity::InsertSubSystemAccess(int GroupID, const ISUuid &SubSystemUID)
{
	ISQuery qInsert(QI_GROUP_ACCESS_SUBSYSTEM);
	qInsert.BindValue(":GroupID", GroupID);
	qInsert.BindValue(":SubSystemUID", SubSystemUID);
	qInsert.Execute();
}
//-----------------------------------------------------------------------------
void ISUserRoleEntity::DeleteSubSystemAccess(int GroupID, const ISUuid &SubSystemUID)
{
	ISQuery qDelete(QD_GROUP_ACCESS_SUBSYSTEM);
	qDelete.BindValue(":GroupID", GroupID);
	qDelete.BindValue(":SubSystemUID", SubSystemUID);
	qDelete.Execute();
}
//-----------------------------------------------------------------------------
bool ISUserRoleEntity::CheckExistTableAccess(int GroupID, const ISUuid &TableUID, const ISUuid &AccessTypeUID)
{
	ISQuery qSelect(QS_GROUP_ACCESS_TABLE_CHECK);
	qSelect.BindValue(":GroupID", GroupID);
	qSelect.BindValue(":TableUID", TableUID);
	qSelect.BindValue(":AccessTypeUID", AccessTypeUID);
	if (qSelect.ExecuteFirst())
	{
		if (qSelect.ReadColumn("count").toInt())
		{
			return true;
		}
	}

	return false;
}
//-----------------------------------------------------------------------------
void ISUserRoleEntity::InsertTableAccess(int GroupID, const ISUuid &TableUID, const ISUuid &AccessTypeUID)
{
	ISQuery qInsert(QI_GROUP_ACCESS_TABLE);
	qInsert.BindValue(":GroupID", GroupID);
	qInsert.BindValue(":TableUID", TableUID);
	qInsert.BindValue(":AccessTypeUID", AccessTypeUID);
	qInsert.Execute();
}
//-----------------------------------------------------------------------------
void ISUserRoleEntity::DeleteTableAccess(int GroupID, const ISUuid &TableUID, const ISUuid &AccessTypeUID)
{
	ISQuery qDelete(QD_GROUP_ACCESS_TABLE);
	qDelete.BindValue(":GroupID", GroupID);
	qDelete.BindValue(":TableUID", TableUID);
	qDelete.BindValue(":AccessTypeUID", AccessTypeUID);
	qDelete.Execute();
}
//-----------------------------------------------------------------------------
bool ISUserRoleEntity::CheckExistSpecialAccess(int GroupID, const ISUuid &SpecialAccessUID)
{
	ISQuery qSelect(QS_GROUP_ACCESS_SPECIAL_CHECK);
	qSelect.BindValue(":GroupID", GroupID);
	qSelect.BindValue(":SpecialAccessUID", SpecialAccessUID);
	if (qSelect.ExecuteFirst())
	{
		if (qSelect.ReadColumn("count").toInt())
		{
			return true;
		}
	}

	return false;
}
//-----------------------------------------------------------------------------
void ISUserRoleEntity::InsertSpecialAccess(int GroupID, const ISUuid &SpecialAccessUID)
{
	ISQuery qInsert(QI_GROUP_ACCESS_SPECIAL);
	qInsert.BindValue(":GroupID", GroupID);
	qInsert.BindValue(":SpecialAccessUID", SpecialAccessUID);
	qInsert.Execute();
}
//-----------------------------------------------------------------------------
void ISUserRoleEntity::DeleteSpecialAccess(int GroupID, const ISUuid &SpecialAccessUID)
{
	ISQuery qDelete(QD_GROUP_ACCESS_SPECIAL);
	qDelete.BindValue(":GroupID", GroupID);
	qDelete.BindValue(":SpecialAccessUID", SpecialAccessUID);
	qDelete.Execute();
}
//-----------------------------------------------------------------------------
void ISUserRoleEntity::Initialize()
{
	if (ISMetaUser::GetInstance().GetData()->System) //≈сли текущий пользователь €вл€етс€ системным
	{
		return;
	}

	ISCountingTime CountingTime;
	InitializeSubSystem();
	InitializeTables();
	InitializeSpecial();
	ISLOGGER_DEBUG(QString("Initialized UserRole %1 msec").arg(CountingTime.GetElapsed()));
}
//-----------------------------------------------------------------------------
bool ISUserRoleEntity::CheckAccessSubSystem(const ISUuid &SubSystemUID)
{
	bool Result = false;
	if (ISMetaUser::GetInstance().GetData()->System || ISMetaUser::GetInstance().GetData()->GroupFullAccess)
	{
		Result = true;
	}
	else
	{
		Result = SubSystems.contains(SubSystemUID);
	}

	return Result;
}
//-----------------------------------------------------------------------------
bool ISUserRoleEntity::CheckAccessTable(const ISUuid &TableUID, const ISUuid &AccessUID)
{
	bool Result = false;
	if (ISMetaUser::GetInstance().GetData()->System || ISMetaUser::GetInstance().GetData()->GroupFullAccess)
	{
		Result = true;
	}
	else
	{
		Result = Tables.value(TableUID).contains(AccessUID);
	}

	return Result;
}
//-----------------------------------------------------------------------------
bool ISUserRoleEntity::CheckAccessSpecial(const ISUuid &SpecialAccessUID)
{
	bool Result = false;
	if (ISMetaUser::GetInstance().GetData()->System || ISMetaUser::GetInstance().GetData()->GroupFullAccess)
	{
		Result = true;
	}
	else
	{
		Result = Specials.contains(SpecialAccessUID);
	}
	
	return Result;
}
//-----------------------------------------------------------------------------
bool ISUserRoleEntity::CheckExistAccesses() const
{
	bool Result = false;
	if (ISMetaUser::GetInstance().GetData()->System || ISMetaUser::GetInstance().GetData()->GroupFullAccess)
	{
		Result = true;
	}
	else
	{
		Result = SubSystems.count() + Tables.count() + Specials.count();
	}

	return Result;
}
//-----------------------------------------------------------------------------
void ISUserRoleEntity::InitializeSubSystem()
{
	ISQuery qSelect(QS_GROUP_ACCESS_SUBSYSTEM);
	qSelect.BindValue(":GroupID", ISMetaUser::GetInstance().GetData()->GroupID);
	if (qSelect.Execute())
	{
		while (qSelect.Next())
		{
			SubSystems.append(ISUuid(qSelect.ReadColumn("gass_subsystem")));
		}
	}
}
//-----------------------------------------------------------------------------
void ISUserRoleEntity::InitializeTables()
{
	ISQuery qSelect(QS_GROUP_ACCESS_TABLE);
	qSelect.BindValue(":GroupID", ISMetaUser::GetInstance().GetData()->GroupID);
	if (qSelect.Execute())
	{
		while (qSelect.Next())
		{
			ISUuid TableUID = qSelect.ReadColumn("gatb_table");
			ISUuid AccessUID = qSelect.ReadColumn("gatb_accesstype");

			if (Tables.contains(TableUID))
			{
				Tables[TableUID].append(AccessUID);
			}
			else
			{
				QVector<ISUuid> Vector;
				Vector.append(AccessUID);
				Tables.insert(TableUID, Vector);
			}
		}
	}
}
//-----------------------------------------------------------------------------
void ISUserRoleEntity::InitializeSpecial()
{
	ISQuery qSelect(QS_GROUP_ACCESS_SPECIAL);
	qSelect.BindValue(":GroupID", ISMetaUser::GetInstance().GetData()->GroupID);
	if (qSelect.Execute())
	{
		while (qSelect.Next())
		{
			Specials.append(ISUuid(qSelect.ReadColumn("gasp_specialaccess")));
		}
	}
}
//-----------------------------------------------------------------------------
