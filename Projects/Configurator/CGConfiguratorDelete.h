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
	bool systems();
	bool subsystems();
	bool tables();
	bool fields();
	bool resources();

protected:
	void ShowResourceConsole(PMetaTable *MetaTable, const ISUuid &ResourceUID);
};
//-----------------------------------------------------------------------------
#endif
