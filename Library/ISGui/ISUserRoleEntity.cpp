#include "ISUserRoleEntity.h"
#include "ISQuery.h"
#include "ISMetaUser.h"
#include "ISMetaSystemsEntity.h"
#include "ISLogger.h"
#include "ISAlgorithm.h"
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
	: ErrorString(NO_ERROR_STRING)
{
	
}
//-----------------------------------------------------------------------------
ISUserRoleEntity::~ISUserRoleEntity()
{

}
//-----------------------------------------------------------------------------
ISUserRoleEntity& ISUserRoleEntity::Instance()
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
QString ISUserRoleEntity::GetErrorString() const
{
	return ErrorString;
}
//-----------------------------------------------------------------------------
bool ISUserRoleEntity::Initialize()
{
	if (!ISMetaUser::Instance().UserData.System) //Если текущий пользователь не является системным - инициализируем
	{
		if (!InitializeSubSystem())
		{
			return false;
		}
		if (!InitializeTables())
		{
			return false;
		}
		if (!InitializeSpecial())
		{
			return false;
		}
	}
	return true;
}
//-----------------------------------------------------------------------------
bool ISUserRoleEntity::CheckAccessSubSystem(const ISUuid &SubSystemUID)
{
	return ISMetaUser::Instance().UserData.System || ISMetaUser::Instance().UserData.GroupFullAccess ?
		true : ISAlgorithm::VectorContains(SubSystems, SubSystemUID);
}
//-----------------------------------------------------------------------------
bool ISUserRoleEntity::CheckAccessTable(const ISUuid &TableUID, const ISUuid &AccessUID)
{
	return ISMetaUser::Instance().UserData.System || ISMetaUser::Instance().UserData.GroupFullAccess ?
		true : ISAlgorithm::VectorContains(Tables[TableUID], AccessUID);
}
//-----------------------------------------------------------------------------
bool ISUserRoleEntity::CheckAccessSpecial(const ISUuid &SpecialAccessUID)
{
	return ISMetaUser::Instance().UserData.System || ISMetaUser::Instance().UserData.GroupFullAccess ?
		true : ISAlgorithm::VectorContains(Specials, SpecialAccessUID);
}
//-----------------------------------------------------------------------------
bool ISUserRoleEntity::CheckExistAccesses() const
{
	return ISMetaUser::Instance().UserData.System || ISMetaUser::Instance().UserData.GroupFullAccess ?
		true : SubSystems.size() + Tables.size() + Specials.size();
}
//-----------------------------------------------------------------------------
bool ISUserRoleEntity::InitializeSubSystem()
{
	ISQuery qSelect(QS_GROUP_ACCESS_SUBSYSTEM);
	qSelect.BindValue(":GroupID", ISMetaUser::Instance().UserData.GroupID);
	bool Result = qSelect.Execute();
	if (Result)
	{
		while (qSelect.Next())
		{
			SubSystems.emplace_back(ISUuid(qSelect.ReadColumn("gass_subsystem")));
		}
	}
	else
	{
		ErrorString = qSelect.GetErrorString();
	}
	return Result;
}
//-----------------------------------------------------------------------------
bool ISUserRoleEntity::InitializeTables()
{
	ISQuery qSelect(QS_GROUP_ACCESS_TABLE);
	qSelect.BindValue(":GroupID", ISMetaUser::Instance().UserData.GroupID);
	bool Result = qSelect.Execute();
	if (Result)
	{
		while (qSelect.Next())
		{
			ISUuid TableUID = qSelect.ReadColumn("gatb_table");
			ISUuid AccessUID = qSelect.ReadColumn("gatb_accesstype");
			Tables.count(TableUID) ? Tables[TableUID].emplace_back(AccessUID) : Tables[TableUID] = { AccessUID };
		}
	}
	else
	{
		ErrorString = qSelect.GetErrorString();
	}
	return Result;
}
//-----------------------------------------------------------------------------
bool ISUserRoleEntity::InitializeSpecial()
{
	ISQuery qSelect(QS_GROUP_ACCESS_SPECIAL);
	qSelect.BindValue(":GroupID", ISMetaUser::Instance().UserData.GroupID);
	bool Result = qSelect.Execute();
	if (Result)
	{
		while (qSelect.Next())
		{
			Specials.emplace_back(ISUuid(qSelect.ReadColumn("gasp_specialaccess")));
		}
	}
	else
	{
		ErrorString = qSelect.GetErrorString();
	}
	return Result;
}
//-----------------------------------------------------------------------------
