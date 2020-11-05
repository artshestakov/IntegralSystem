#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISTypedefs.h"
//-----------------------------------------------------------------------------
class ISUserRoleEntity
{
public:
	static ISUserRoleEntity& Instance();

	static bool CheckExistSubSystemAccess(int GroupID, const ISUuid &SubSystemUID); //ѕроверить наличие права на подсистему в базе
	static void InsertSubSystemAccess(int GroupID, const ISUuid &SubSystemUID); //ƒобавить право на подсистему в базу
	static void DeleteSubSystemAccess(int GroupID, const ISUuid &SubSystemUID); //”далить право на подсистему из базы
	
	static bool CheckExistTableAccess(int GroupID, const QString &TableName, int AccessTypeID); //ѕроверить наличие права на таблицу в базе
	static void InsertTableAccess(int GroupID, const QString &TableName, int AccessTypeID);
	static void DeleteTableAccess(int GroupID, const QString &TableName, int AccessTypeID);
	
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
	bool CheckExistAccesses() const; //ѕроверить наличие прав доступа

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
