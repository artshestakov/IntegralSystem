#include "StdAfx.h"
#include "ISMetaUserData.h"
#include "ISSystem.h"
//-----------------------------------------------------------------------------
ISMetaUserData::ISMetaUserData(QObject *parent) : QObject(parent)
{
	System = false;
	ID = 0;
	GroupID = 0;
	AccessAllowed = false;
	FixedInactive = false;
	InactiveTimeout = 0;
	GroupFullAccess = false;
}
//-----------------------------------------------------------------------------
ISMetaUserData::~ISMetaUserData()
{

}
//-----------------------------------------------------------------------------
