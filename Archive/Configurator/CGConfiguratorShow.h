#pragma once
#ifndef _CGCONFIGURATORSHOW_H_INCLUDED
#define _CGCONFIGURATORSHOW_H_INCLUDED
//-----------------------------------------------------------------------------
#include "CGConfiguratorBase.h"
#include "PMetaClass.h"
//-----------------------------------------------------------------------------
class CGConfiguratorShow : public CGConfiguratorBase
{
	Q_OBJECT

public:
	Q_INVOKABLE CGConfiguratorShow();
	virtual ~CGConfiguratorShow();

public slots:
	bool oldobjects();
	bool databaseinfo();

private:
	bool oldtables(int &Count);
	bool oldfields(int &Count);
	bool oldresources(int &Count);
	bool oldsequence(int &Count);
	bool oldindexes(int &Count);
	bool oldforeigns(int &Count);

private:
	PMetaTable* FoundTable(const QString &TableName);
	PMetaField* FoundField(PMetaTable *MetaTable, const QString &ColumnName);
};
//-----------------------------------------------------------------------------
#endif
