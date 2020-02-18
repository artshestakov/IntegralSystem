#pragma once
//-----------------------------------------------------------------------------
#include "PMetaClass.h"
//-----------------------------------------------------------------------------
class CGClass
{
public:
	static bool CheckExistClass(PMetaClassTable *MetaTable);
	static void InsertClass(PMetaClassTable *MetaTable);
	static void UpdateClass(PMetaClassTable *MetaTable);
};
//-----------------------------------------------------------------------------
