#pragma once
//-----------------------------------------------------------------------------
#include "isconfigurator_global.h"
#include "CGConfiguratorBase.h"
#include "PMetaClassTable.h"
//-----------------------------------------------------------------------------
class ISCONFIGURATOR_EXPORT CGConfiguratorDelete : public CGConfiguratorBase
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
