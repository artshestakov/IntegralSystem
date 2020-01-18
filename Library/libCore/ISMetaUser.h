#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISTypes.h"
//-----------------------------------------------------------------------------
class ISMetaUser
{
public:
	ISMetaUser(const ISMetaUser &) = delete;
	ISMetaUser(ISMetaUser &&) = delete;
	ISMetaUser &operator=(const ISMetaUser &) = delete;
	ISMetaUser &operator=(ISMetaUser &&) = delete;
	~ISMetaUser();

	static ISMetaUser& GetInstance();

	void Initialize(const QString &login, const QString &password);

	ISMetaUserData* GetData();
	bool CheckPassword(const QString &EnteredPassword);
	QString GetCurrentStatus(int UserID) const; //Получить статус пользователя

private:
	ISMetaUser();

private:
	ISMetaUserData *UserData;
};
//-----------------------------------------------------------------------------
