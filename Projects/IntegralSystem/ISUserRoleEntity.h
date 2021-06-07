#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISTypedefsGUI.h"
//-----------------------------------------------------------------------------
class ISUserRoleEntity
{
public:
    static ISUserRoleEntity& Instance();

    void InitializeTables(const QVariantMap &VariantMap);
    void InitializeSpecial(const QVariantList &VariantList);

    bool CheckAccessTable(const QString &TableName, const QString &AccessUID);
    bool CheckAccessSpecial(const QString &SpecialAccessUID);

private:
    ISUserRoleEntity();
    ~ISUserRoleEntity();
    ISUserRoleEntity(const ISUserRoleEntity&) = delete;
    ISUserRoleEntity(ISUserRoleEntity&&) = delete;
    ISUserRoleEntity& operator=(const ISUserRoleEntity&) = delete;
    ISUserRoleEntity& operator=(ISUserRoleEntity&&) = delete;

private:
    std::map<QString, ISVectorStringQt> Tables;
    ISVectorStringQt Specials;
};
//-----------------------------------------------------------------------------
