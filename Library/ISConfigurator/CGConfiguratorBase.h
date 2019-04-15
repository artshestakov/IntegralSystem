#pragma once
//-----------------------------------------------------------------------------
#include "isconfigurator_global.h"
//-----------------------------------------------------------------------------
class ISCONFIGURATOR_EXPORT CGConfiguratorBase : public QObject
{
	Q_OBJECT

signals:
	void ProgressMessage(const QString &Message);

public:
	CGConfiguratorBase();
	virtual ~CGConfiguratorBase();

protected:
	void Progress(const QString &ProgressName, int Step, int Steps);
};
//-----------------------------------------------------------------------------
