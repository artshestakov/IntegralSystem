#pragma once
//-----------------------------------------------------------------------------
#include "isconfigurator_global.h"
#include "CGConfiguratorBase.h"
//-----------------------------------------------------------------------------
class ISCONFIGURATOR_EXPORT CGConfiguratorCreate : public CGConfiguratorBase
{
	Q_OBJECT

public:
	Q_INVOKABLE CGConfiguratorCreate();
	virtual ~CGConfiguratorCreate();

public slots:
	void database();
	void systemuser();
	void databasesettings();
	void resources();
};
//-----------------------------------------------------------------------------
