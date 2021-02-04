#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISTypedefs.h"
//-----------------------------------------------------------------------------
class ISUserRoleEntity
{
public:
	static ISUserRoleEntity& Instance();

	void InitializeTables(const QVariantMap &VariantMap);
	void InitializeSpecial(const QVariantList &VariantList);

	bool CheckAccessTable(const QString &TableName, const ISUuid &AccessUID);
	bool CheckAccessSpecial(const ISUuid &SpecialAccessUID);

private:
	ISUserRoleEntity();
	~ISUserRoleEntity();
	ISUserRoleEntity(const ISUserRoleEntity&) = delete;
	ISUserRoleEntity(ISUserRoleEntity&&) = delete;
	ISUserRoleEntity& operator=(const ISUserRoleEntity&) = delete;
	ISUserRoleEntity& operator=(ISUserRoleEntity&&) = delete;

private:
	std::map<QString, ISVectorUID> Tables;
	ISVectorUID Specials;
};
//-----------------------------------------------------------------------------
