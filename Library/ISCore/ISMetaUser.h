#pragma once
#ifndef _ISMETAUSER_H_INCLUDED
#define _ISMETAUSER_H_INCLUDED
//-----------------------------------------------------------------------------
#include "iscore_global.h"
#include "ISStructs.h"
//-----------------------------------------------------------------------------
#define CURRENT_USER_ID ISMetaUser::Instance().UserData.ID
//-----------------------------------------------------------------------------
class ISCORE_EXPORT ISMetaUser
{
public:
	ISMetaUserData UserData;

public:
	static ISMetaUser& Instance();

	QString GetErrorString() const;
	bool Initialize();

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
