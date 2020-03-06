#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISTypedefs.h"
//-----------------------------------------------------------------------------
class ISUserRoleEntity
{
public:
	ISUserRoleEntity(const ISUserRoleEntity &) = delete;
	ISUserRoleEntity(ISUserRoleEntity &&) = delete;
	ISUserRoleEntity &operator=(const ISUserRoleEntity &) = delete;
	ISUserRoleEntity &operator=(ISUserRoleEntity &&) = delete;
	~ISUserRoleEntity();

	static ISUserRoleEntity& GetInstance();

	static bool CheckExistSubSystemAccess(int GroupID, const ISUuid &SubSystemUID); //��������� ������� ����� �� ���������� � ����
	static void InsertSubSystemAccess(int GroupID, const ISUuid &SubSystemUID); //�������� ����� �� ���������� � ����
	static void DeleteSubSystemAccess(int GroupID, const ISUuid &SubSystemUID); //������� ����� �� ���������� �� ����
	
	static bool CheckExistTableAccess(int GroupID, const ISUuid &TableUID, const ISUuid &AccessTypeUID); //��������� ������� ����� �� ������� � ����
	static void InsertTableAccess(int GroupID, const ISUuid &TableUID, const ISUuid &AccessTypeUID);
	static void DeleteTableAccess(int GroupID, const ISUuid &TableUID, const ISUuid &AccessTypeUID);
	
	static bool CheckExistSpecialAccess(int GroupID, const ISUuid &SpecialAccessUID);
	static void InsertSpecialAccess(int GroupID, const ISUuid &SpecialAccessUID);
	static void DeleteSpecialAccess(int GroupID, const ISUuid &SpecialAccessUID);

	void Initialize();

	bool CheckAccessSubSystem(const ISUuid &SubSystemUID);
	bool CheckAccessTable(const ISUuid &TableUID, const ISUuid &AccessUID);
	bool CheckAccessSpecial(const ISUuid &SpecialAccessUID);
	bool CheckExistAccesses() const; //��������� ������� ���� �������

protected:
	void InitializeSubSystem();
	void InitializeTables();
	void InitializeSpecial();

private:
	ISUserRoleEntity();

	ISVectorUID SubSystems;
	QMap<ISUuid, ISVectorUID> Tables;
	ISVectorUID Specials;
};
//-----------------------------------------------------------------------------
