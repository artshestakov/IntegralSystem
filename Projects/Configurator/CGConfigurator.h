#pragma once
#ifndef _CGCONFIGURATOR_H_INCLUDED
#define _CGCONFIGURATOR_H_INCLUDED
//-----------------------------------------------------------------------------
#include "CGConfiguratorBase.h"
#include "CGSection.h"
//-----------------------------------------------------------------------------
class CGConfigurator : public CGConfiguratorBase
{
	Q_OBJECT

public:
	Q_INVOKABLE CGConfigurator(const QVector<CGSection*> &arguments);
	virtual ~CGConfigurator();

public slots:
	void help(); //Показать помощь

private:
	QVector<CGSection*> Arguments;
};
//-----------------------------------------------------------------------------
#endif
