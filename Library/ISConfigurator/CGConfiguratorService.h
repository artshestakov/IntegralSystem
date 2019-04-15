#pragma once
//-----------------------------------------------------------------------------
#include "isconfigurator_global.h"
#include "CGConfiguratorBase.h"
//-----------------------------------------------------------------------------
class ISCONFIGURATOR_EXPORT CGConfiguratorService : public CGConfiguratorBase
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
