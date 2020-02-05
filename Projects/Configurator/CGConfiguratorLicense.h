#pragma once
#ifndef _CGCONFIGURATORLICENSE_H_INCLUDED
#define _CGCONFIGURATORLICENSE_H_INCLUDED
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
#endif
