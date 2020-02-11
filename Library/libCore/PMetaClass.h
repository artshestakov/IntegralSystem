#pragma once
#ifndef _PMETACLASS_H_INCLUDED
#define _PMETACLASS_H_INCLUDED
//-----------------------------------------------------------------------------
#include "libCore_global.h"
//-----------------------------------------------------------------------------
class PMetaClass
{
public:
	PMetaClass(const QString &type_object);
	~PMetaClass();

	QString GetTypeObject() const; //Получить тип объекта

private:
	QString TypeObject; //Тип объекта
};
//-----------------------------------------------------------------------------
#endif
