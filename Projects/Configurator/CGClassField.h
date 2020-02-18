#pragma once
//-----------------------------------------------------------------------------
#include "PMetaClass.h"
//-----------------------------------------------------------------------------
class CGClassField
{
public:
	static bool CheckExistClassField(PMetaClassField *MetaField);
	static void InsertClassField(const ISUuid &ClassUID, PMetaClassField *MetaField);
	static void UpdateClassField(const ISUuid &ClassUID, PMetaClassField *MetaField);
};
//-----------------------------------------------------------------------------
