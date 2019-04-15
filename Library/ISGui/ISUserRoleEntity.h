#pragma once
//-----------------------------------------------------------------------------
#include "isgui_global.h"
#include "PMetaUserPermission.h"
#include "ISNamespace.h"
#include "ISTypes.h"
#include "ISUuid.h"
//-----------------------------------------------------------------------------
class ISGUI_EXPORT ISUserRoleEntity : public QObject
{
	Q_OBJECT

public:
	ISUserRoleEntity(const ISUserRoleEntity &) = delete;
	ISUserRoleEntity(ISUserRoleEntity &&) = delete;
	ISUserRoleEntity &operator=(const ISUserRoleEntity &) = delete;
	ISUserRoleEntity &operator=(ISUserRoleEntity &&) = delete;
	~ISUserRoleEntity();

	static ISUserRoleEntity& GetInstance();

	static bool CheckExistSubSystemAccess(int GroupID, const ISUuid &SubSystemUID); //ѕроверить наличие права на подсистему в базе
	static void InsertSubSystemAccess(int GroupID, const ISUuid &SubSystemUID); //ƒобавить право на подсистему в базу
	static void DeleteSubSystemAccess(int GroupID, const ISUuid &SubSystemUID); //”далить право на подсистему из базы
	
	static bool CheckExistTableAccess(int GroupID, const ISUuid &TableUID, const ISUuid &AccessTypeUID); //ѕроверить наличие права на таблицу в базе
	static void InsertTableAccess(int GroupID, const ISUuid &TableUID, const ISUuid &AccessTypeUID);
	static void DeleteTableAccess(int GroupID, const ISUuid &TableUID, const ISUuid &AccessTypeUID);
	
	static bool CheckExistSpecialAccess(int GroupID, const ISUuid &SpecialAccessUID);
	static void InsertSpecialAccess(int GroupID, const ISUuid &SpecialAccessUID);
	static void DeleteSpecialAccess(int GroupID, const ISUuid &SpecialAccessUID);

	void Initialize();

	bool CheckAccessSubSystem(const ISUuid &SubSystemUID);
	bool CheckAccessTable(const ISUuid &TableUID, const ISUuid &AccessUID);
	bool CheckAccessSpecial(const ISUuid &SpecialAccessUID);
	bool CheckExistAccesses() const; //ѕроверить наличие прав доступа

protected:
	void InitializeSubSystem();
	void InitializeTables();
	void InitializeSpecial();

private:
	ISUserRoleEntity();

	QVector<ISUuid> SubSystems;
	QMap<ISUuid, QVector<ISUuid>> Tables;
	QVector<ISUuid> Specials;
};
//-----------------------------------------------------------------------------
