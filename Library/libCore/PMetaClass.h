#pragma once
#ifndef _PMETACLASS_H_INCLUDED
#define _PMETACLASS_H_INCLUDED
//-----------------------------------------------------------------------------
#include "libCore_global.h"
//-----------------------------------------------------------------------------
struct PMetaClass
{
	PMetaClass(const QString &type_object) : TypeObject(type_object) { }

	QString TypeObject; //Тип объекта
};
//-----------------------------------------------------------------------------
#endif
