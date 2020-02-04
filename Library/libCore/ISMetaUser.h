#pragma once
#ifndef _ISMETAUSER_H_INCLUDED
#define _ISMETAUSER_H_INCLUDED
//-----------------------------------------------------------------------------
#include "libCore_global.h"
#include "ISTypes.h"
//-----------------------------------------------------------------------------
class LIBCORE_EXPORT ISMetaUser
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
#endif
