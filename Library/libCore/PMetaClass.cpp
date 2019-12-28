#include "StdAfx.h"
#include "PMetaClass.h"
//-----------------------------------------------------------------------------
PMetaClass::PMetaClass(QObject *parent) : QObject(parent)
{

}
//-----------------------------------------------------------------------------
PMetaClass::~PMetaClass()
{

}
//-----------------------------------------------------------------------------
void PMetaClass::SetName(const QString &name)
{
	Name = name;
}
//-----------------------------------------------------------------------------
QString PMetaClass::GetName() const
{
	return Name;
}
//-----------------------------------------------------------------------------
void PMetaClass::SetUID(const QString &uid)
{
	UID = uid;
}
//-----------------------------------------------------------------------------
QString PMetaClass::GetUID() const
{
	return UID;
}
//-----------------------------------------------------------------------------
void PMetaClass::SetTypeObject(const QString &type_object)
{
	TypeObject = type_object;
}
//-----------------------------------------------------------------------------
QString PMetaClass::GetTypeObject() const
{
	return TypeObject;
}
//-----------------------------------------------------------------------------
