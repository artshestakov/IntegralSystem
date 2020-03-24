#pragma once
#ifndef _CGCONFIGURATORSERVICE_H_INCLUDED
#define _CGCONFIGURATORSERVICE_H_INCLUDED
//-----------------------------------------------------------------------------
#include "CGConfiguratorBase.h"
//-----------------------------------------------------------------------------
class CGConfiguratorService : public CGConfiguratorBase
{
	Q_OBJECT

public:
	Q_INVOKABLE CGConfiguratorService();
	virtual ~CGConfiguratorService();

public slots:
	bool reindex();
	bool vacuum();
	bool vacuumanalyze();
	bool vacuumfull();
	bool cleartable();
};
//-----------------------------------------------------------------------------
#endif
