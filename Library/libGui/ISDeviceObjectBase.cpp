#include "StdAfx.h"
#include "ISDeviceObjectBase.h"
//-----------------------------------------------------------------------------
ISDeviceObjectBase::ISDeviceObjectBase(int device_user_id, QObject *parent) : QObject(parent)
{
	DeviceUserID = device_user_id;
}
//-----------------------------------------------------------------------------
ISDeviceObjectBase::~ISDeviceObjectBase()
{

}
//-----------------------------------------------------------------------------
int ISDeviceObjectBase::GetDeviceID() const
{
	return DeviceUserID;
}
//-----------------------------------------------------------------------------
