#pragma once
//-----------------------------------------------------------------------------
#include "CGConfiguratorBase.h"
//-----------------------------------------------------------------------------
class CGConfiguratorLicense : public CGConfiguratorBase
{
	Q_OBJECT

public:
	Q_INVOKABLE CGConfiguratorLicense();
	virtual ~CGConfiguratorLicense();

public slots:
	void create();
	void show();
	void update();
};
//-----------------------------------------------------------------------------
