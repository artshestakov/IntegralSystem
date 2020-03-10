#pragma once
//-----------------------------------------------------------------------------
#include "PMetaClass.h"
//-----------------------------------------------------------------------------
class CGTable
{
public:
	static bool CreateTable(PMetaClassTable *MetaTable, QString &ErrorString);
	static bool UpdateTable(PMetaClassTable *MetaTable, QString &ErrorString);
	static bool CheckExistTable(PMetaClassTable *MetaTable, bool &Exist, QString &ErrorString);

protected:
	static bool AlterExistFields(PMetaClassTable *MetaTable, QString &ErrorString);
	static bool CreateNewFields(PMetaClassTable *MetaTable, QString &ErrorString);
};
//-----------------------------------------------------------------------------
