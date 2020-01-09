#pragma once
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
	void reindex();
	void vacuum();
	void vacuumanalyze();
	void vacuumfull();
	void cleartable();
};
//-----------------------------------------------------------------------------
