#pragma once
#ifndef _CGCONFIGURATORBASE_H_INCLUDED
#define _CGCONFIGURATORBASE_H_INCLUDED
//-----------------------------------------------------------------------------
#include "PMetaClass.h"
//-----------------------------------------------------------------------------
class CGConfiguratorBase : public QObject
{
	Q_OBJECT

public:
	CGConfiguratorBase();
	virtual ~CGConfiguratorBase();

	QString GetErrorString() const;

protected:
	void Progress(const QString &ObjectTypeName, size_t Step, size_t TotalStep, const QString &Description = QString());

protected:
	QString ErrorString;
};
//-----------------------------------------------------------------------------
#endif
