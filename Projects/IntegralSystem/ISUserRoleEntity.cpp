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
    for (const auto &MapItem : VariantMap.toStdMap())
    {
        for (const QVariant &AccessUID : MapItem.second.toList())
        {
            Tables.count(MapItem.first) ? Tables[MapItem.first].emplace_back(AccessUID.toString()) : Tables[MapItem.first] = { AccessUID.toString() };
        }
    }
}
//-----------------------------------------------------------------------------
void ISUserRoleEntity::InitializeSpecial(const QVariantList &VariantList)
{
    for (const QVariant &SpecialAccessUID : VariantList)
    {
        Specials.emplace_back(SpecialAccessUID.toString());
    }
}
//-----------------------------------------------------------------------------
bool ISUserRoleEntity::CheckAccessTable(const QString &TableName, const QString &AccessUID)
{
    return ISBuffer::Instance().CurrentUserInfo.System || ISBuffer::Instance().CurrentUserInfo.GroupFullAccess ?
        true : ISAlgorithm::VectorContains(Tables[TableName], AccessUID);
}
//-----------------------------------------------------------------------------
bool ISUserRoleEntity::CheckAccessSpecial(const QString &SpecialAccessUID)
{
    return ISBuffer::Instance().CurrentUserInfo.System || ISBuffer::Instance().CurrentUserInfo.GroupFullAccess ?
        true : ISAlgorithm::VectorContains(Specials, SpecialAccessUID);
}
//-----------------------------------------------------------------------------
