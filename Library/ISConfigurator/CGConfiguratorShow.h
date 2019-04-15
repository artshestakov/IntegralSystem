#pragma once
//-----------------------------------------------------------------------------
#include "isconfigurator_global.h"
#include "CGConfiguratorBase.h"
#include "PMetaClassTable.h"
#include "PMetaClassField.h"
//-----------------------------------------------------------------------------
class ISCONFIGURATOR_EXPORT CGConfiguratorShow : public CGConfiguratorBase
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
	PMetaClassTable * FoundTable(const QString &TableName);
	PMetaClassField* FoundField(PMetaClassTable *MetaTable, const QString &ColumnName);
};
//-----------------------------------------------------------------------------
