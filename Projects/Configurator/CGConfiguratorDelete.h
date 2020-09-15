#pragma once
#ifndef _CGCONFIGURATORDELETE_H_INCLUDED
#define _CGCONFIGURATORDELETE_H_INCLUDED
//-----------------------------------------------------------------------------
#include "CGConfiguratorBase.h"
#include "PMetaClass.h"
//-----------------------------------------------------------------------------
class CGConfiguratorDelete : public CGConfiguratorBase
{
	Q_OBJECT

public:
	Q_INVOKABLE CGConfiguratorDelete();
	virtual ~CGConfiguratorDelete();

public slots:
	bool indexes();
	bool foreigns();
	bool oldtables();
	bool oldfields();
	bool oldresources();
	bool oldsequence();

protected:
	void ShowResourceConsole(PMetaTable *MetaTable, const ISUuid &ResourceUID);
};
//-----------------------------------------------------------------------------
#endif
