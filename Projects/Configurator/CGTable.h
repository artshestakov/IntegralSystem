#pragma once
//-----------------------------------------------------------------------------
#include "PMetaClassTable.h"
//-----------------------------------------------------------------------------
class CGTable : public QObject
{
	Q_OBJECT

public:
	CGTable();
	virtual ~CGTable();

	static void CreateTable(PMetaClassTable *MetaTable, QString &ErrorString);
	static void UpdateTable(PMetaClassTable *MetaTable);
	static bool CheckExistTable(PMetaClassTable *MetaTable);

protected:
	static void AlterExistFields(PMetaClassTable *MetaTable);
	static void CreateNewFields(PMetaClassTable *MetaTable);
};
//-----------------------------------------------------------------------------
