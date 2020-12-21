#include "ISUserRoleEntity.h"
#include "ISQuery.h"
#include "ISMetaUser.h"
#include "ISMetaSystemsEntity.h"
#include "ISLogger.h"
#include "ISAlgorithm.h"
//-----------------------------------------------------------------------------
static QString QS_GROUP_ACCESS_SPECIAL_CHECK = PREPARE_QUERY("SELECT COUNT(*) "
															 "FROM _groupaccessspecial "
															 "WHERE gasp_group = :GroupID "
															 "AND gasp_specialaccess = :SpecialAccessID");
//-----------------------------------------------------------------------------
static QString QI_GROUP_ACCESS_SPECIAL = PREPARE_QUERY("INSERT INTO _groupaccessspecial(gasp_group, gasp_specialaccess) "
													   "VALUES(:GroupID, :SpecialAccessID)");
//-----------------------------------------------------------------------------
static QString QD_GROUP_ACCESS_SPECIAL = PREPARE_QUERY("DELETE FROM _groupaccessspecial "
													   "WHERE gasp_group = :GroupID "
													   "AND gasp_specialaccess = :SpecialAccessID");
//-----------------------------------------------------------------------------
static QString QS_GROUP_ACCESS_SUBSYSTEM = PREPARE_QUERY("SELECT gass_subsystem "
														 "FROM _groupaccesssubsystem "
														 "WHERE gass_group = :GroupID");
//-----------------------------------------------------------------------------
static QString QS_GROUP_ACCESS_TABLE = PREPARE_QUERY("SELECT gatb_table, gatt_uid "
													 "FROM _groupaccesstable "
													 "LEFT JOIN _groupaccesstabletype ON gatt_id = gatb_AccessType "
													 "WHERE gatb_group = :GroupID");
//-----------------------------------------------------------------------------
static QString QS_GROUP_ACCESS_SPECIAL = PREPARE_QUERY("SELECT gast_uid "
													   "FROM _groupaccessspecial "
													   "LEFT JOIN _groupaccessspecialtype ON gast_id = gasp_specialaccess "
													   "WHERE gasp_group = :GroupID");
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
bool ISUserRoleEntity::CheckExistSpecialAccess(int GroupID, int SpecialAccessID)
{
	ISQuery qSelect(QS_GROUP_ACCESS_SPECIAL_CHECK);
	qSelect.BindValue(":GroupID", GroupID);
	qSelect.BindValue(":SpecialAccessID", SpecialAccessID);
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
void ISUserRoleEntity::InsertSpecialAccess(int GroupID, int SpecialAccessID)
{
	ISQuery qInsert(QI_GROUP_ACCESS_SPECIAL);
	qInsert.BindValue(":GroupID", GroupID);
	qInsert.BindValue(":SpecialAccessID", SpecialAccessID);
	qInsert.Execute();
}
//-----------------------------------------------------------------------------
void ISUserRoleEntity::DeleteSpecialAccess(int GroupID, int SpecialAccessID)
{
	ISQuery qDelete(QD_GROUP_ACCESS_SPECIAL);
	qDelete.BindValue(":GroupID", GroupID);
	qDelete.BindValue(":SpecialAccessID", SpecialAccessID);
	qDelete.Execute();
}
//-----------------------------------------------------------------------------
QString ISUserRoleEntity::GetErrorString() const
{
	return ErrorString;
}
//-----------------------------------------------------------------------------
void ISUserRoleEntity::InitializeTables(const QVariantMap &VariantMap)
{
	for (const auto &MapItem: VariantMap.toStdMap())
	{
		for (const QVariant &AccessUID : MapItem.second.toList())
		{
			Tables.count(MapItem.first) ? Tables[MapItem.first].emplace_back(AccessUID) : Tables[MapItem.first] = { AccessUID };
		}
	}
}
//-----------------------------------------------------------------------------
void ISUserRoleEntity::InitializeSpecial(const QVariantList &VariantList)
{
	for (const QVariant &SpecialAccessUID : VariantList)
	{
		Specials.emplace_back(SpecialAccessUID);
	}
}
//-----------------------------------------------------------------------------
bool ISUserRoleEntity::Initialize()
{
	if (!ISMetaUser::Instance().UserData.System) //���� ������� ������������ �� �������� ��������� - ��������������
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
bool ISUserRoleEntity::CheckAccessTable(const QString &TableName, const ISUuid &AccessUID)
{
	return ISMetaUser::Instance().UserData.System || ISMetaUser::Instance().UserData.GroupFullAccess ?
		true : ISAlgorithm::VectorContains(Tables[TableName], AccessUID);
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
			QString TableName = qSelect.ReadColumn("gatb_table").toString();
			ISUuid AccessUID = qSelect.ReadColumn("gatt_uid");
			Tables.count(TableName) ? Tables[TableName].emplace_back(AccessUID) : Tables[TableName] = { AccessUID };
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
			Specials.emplace_back(ISUuid(qSelect.ReadColumn("gast_uid")));
		}
	}
	else
	{
		ErrorString = qSelect.GetErrorString();
	}
	return Result;
}
//-----------------------------------------------------------------------------
