#pragma once
//-----------------------------------------------------------------------------
#include "PMetaClass.h"
//-----------------------------------------------------------------------------
class CGTable
{
public:
	static bool CreateTable(PMetaTable *MetaTable, QString &ErrorString);
	static bool UpdateTable(PMetaTable *MetaTable, QString &ErrorString);
	static bool CheckExistTable(PMetaTable *MetaTable, bool &Exist, QString &ErrorString);

protected:
	static bool AlterExistFields(PMetaTable *MetaTable, QString &ErrorString);
	static bool CreateNewFields(PMetaTable *MetaTable, QString &ErrorString);
	static bool DeleteOldFields(PMetaTable *MetaTable, QString &ErrorString);
};
//-----------------------------------------------------------------------------
