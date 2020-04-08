#pragma once
#ifndef _ISMETAUSER_H_INCLUDED
#define _ISMETAUSER_H_INCLUDED
//-----------------------------------------------------------------------------
#include "iscore_global.h"
#include "ISStructs.h"
//-----------------------------------------------------------------------------
class ISCORE_EXPORT ISMetaUser
{
public:
	ISMetaUserData *UserData;

public:
	static ISMetaUser& Instance();
	QString GetErrorString() const;
	bool Initialize(const QString &login, const QString &password);

	bool CheckPassword(const QString &EnteredPassword);

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
