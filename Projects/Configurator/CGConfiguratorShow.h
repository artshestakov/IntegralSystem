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
	void obsoletetables();
	void obsoletefields();
	void obsoleteresources();
	void obsoletesequence();
	void config();
	void databasesize();

private:
	PMetaTable * FoundTable(const QString &TableName);
	PMetaField* FoundField(PMetaTable *MetaTable, const QString &ColumnName);
};
//-----------------------------------------------------------------------------
#endif
