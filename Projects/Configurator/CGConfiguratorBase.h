#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
//-----------------------------------------------------------------------------
class CGConfiguratorBase : public QObject
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
