#pragma once
//-----------------------------------------------------------------------------
#include "CGConfiguratorBase.h"
//-----------------------------------------------------------------------------
class CGConfiguratorCreate : public CGConfiguratorBase
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
