#pragma once
//-----------------------------------------------------------------------------
#include "isconfigurator_global.h"
#include "CGConfiguratorBase.h"
//-----------------------------------------------------------------------------
class ISCONFIGURATOR_EXPORT CGConfiguratorLicense : public CGConfiguratorBase
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
