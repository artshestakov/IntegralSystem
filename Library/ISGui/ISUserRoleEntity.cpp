#include "ISUserRoleEntity.h"
#include "ISAlgorithm.h"
#include "ISBuffer.h"
//-----------------------------------------------------------------------------
ISUserRoleEntity::ISUserRoleEntity()
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
bool ISUserRoleEntity::CheckAccessTable(const QString &TableName, const ISUuid &AccessUID)
{
	return ISBuffer::Instance().CurrentUserInfo.System || ISBuffer::Instance().CurrentUserInfo.GroupFullAccess ?
		true : ISAlgorithm::VectorContains(Tables[TableName], AccessUID);
}
//-----------------------------------------------------------------------------
bool ISUserRoleEntity::CheckAccessSpecial(const ISUuid &SpecialAccessUID)
{
	return ISBuffer::Instance().CurrentUserInfo.System || ISBuffer::Instance().CurrentUserInfo.GroupFullAccess ?
		true : ISAlgorithm::VectorContains(Specials, SpecialAccessUID);
}
//-----------------------------------------------------------------------------
