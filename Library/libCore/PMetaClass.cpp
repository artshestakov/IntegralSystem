#include "PMetaClass.h"
//-----------------------------------------------------------------------------
PMetaClass::PMetaClass(const QString &type_object)
	: TypeObject(type_object)
{

}
//-----------------------------------------------------------------------------
PMetaClass::~PMetaClass()
{

}
//-----------------------------------------------------------------------------
QString PMetaClass::GetTypeObject() const
{
	return TypeObject;
}
//-----------------------------------------------------------------------------
