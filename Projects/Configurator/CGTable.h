#pragma once
//-----------------------------------------------------------------------------
#include "PMetaClass.h"
//-----------------------------------------------------------------------------
class CGTable
{
public:
	static bool CreateTable(PMetaClassTable *MetaTable, QString &ErrorString);
	static bool UpdateTable(PMetaClassTable *MetaTable);
	static bool CheckExistTable(PMetaClassTable *MetaTable);

protected:
	static void AlterExistFields(PMetaClassTable *MetaTable);
	static void CreateNewFields(PMetaClassTable *MetaTable);
};
//-----------------------------------------------------------------------------
