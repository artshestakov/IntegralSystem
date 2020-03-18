#pragma once
#ifndef _ISMETAUSER_H_INCLUDED
#define _ISMETAUSER_H_INCLUDED
//-----------------------------------------------------------------------------
#include "libCore_global.h"
#include "ISStructs.h"
//-----------------------------------------------------------------------------
class LIBCORE_EXPORT ISMetaUser
{
public:
	ISMetaUserData *UserData;

public:
	static ISMetaUser& Instance();
	QString GetErrorString() const;
	bool Initialize(const QString &login, const QString &password);

	bool CheckPassword(const QString &EnteredPassword);
	QString GetCurrentStatus(int UserID) const; //Получить статус пользователя

private:
	ISMetaUser();
	~ISMetaUser();
	ISMetaUser(ISMetaUser const &) {};
	ISMetaUser& operator=(ISMetaUser const&) { return *this; };

private:
	QString ErrorString;
};
//-----------------------------------------------------------------------------
#endif
