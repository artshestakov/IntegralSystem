#pragma once
#ifndef _CGCONFIGURATORCREATE_H_INCLUDED
#define _CGCONFIGURATORCREATE_H_INCLUDED
//-----------------------------------------------------------------------------
#include "CGConfiguratorBase.h"
//-----------------------------------------------------------------------------
class CGConfiguratorCreate : public CGConfiguratorBase
{
	Q_OBJECT

public:
	Q_INVOKABLE CGConfiguratorCreate();
	virtual ~CGConfiguratorCreate();

public slots:
	void resources();
};
//-----------------------------------------------------------------------------
#endif
