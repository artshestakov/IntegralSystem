#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISTypedefs.h"
//-----------------------------------------------------------------------------
class ISUserRoleEntity
{
public:
	static ISUserRoleEntity& Instance();

	static bool CheckExistSpecialAccess(int GroupID, int SpecialAccessID);
	static void InsertSpecialAccess(int GroupID, int SpecialAccessID);
	static void DeleteSpecialAccess(int GroupID, int SpecialAccessID);

	QString GetErrorString() const;
	void InitializeTables(const QVariantMap &VariantMap);
	void InitializeSpecial(const QVariantList &VariantList);
	bool Initialize();

	bool CheckAccessSubSystem(const ISUuid &SubSystemUID);
	bool CheckAccessTable(const QString &TableName, const ISUuid &AccessUID);
	bool CheckAccessSpecial(const ISUuid &SpecialAccessUID);
	bool CheckExistAccesses() const; //Проверить наличие прав доступа

private:
	bool InitializeSubSystem();
	bool InitializeTables();
	bool InitializeSpecial();

private:
	ISUserRoleEntity();
	~ISUserRoleEntity();
	ISUserRoleEntity(ISUserRoleEntity const &) {};
	ISUserRoleEntity& operator=(ISUserRoleEntity const&) { return *this; };

private:
	QString ErrorString;
	ISVectorUID SubSystems;
	std::map<QString, ISVectorUID> Tables;
	ISVectorUID Specials;
};
//-----------------------------------------------------------------------------
