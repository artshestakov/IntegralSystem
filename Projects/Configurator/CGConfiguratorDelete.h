#pragma once
//-----------------------------------------------------------------------------
#include "CGConfiguratorBase.h"
#include "PMetaClassTable.h"
//-----------------------------------------------------------------------------
class CGConfiguratorDelete : public CGConfiguratorBase
{
	Q_OBJECT

public:
	Q_INVOKABLE CGConfiguratorDelete();
	virtual ~CGConfiguratorDelete();

public slots:
	void indexes();
	void foreigns();
	void systems();
	void subsystems();
	void tables();
	void fields();
	void resources();

protected:
	void ShowResourceConsole(PMetaClassTable *MetaTable, const ISUuid &ResourceUID);
};
//-----------------------------------------------------------------------------
