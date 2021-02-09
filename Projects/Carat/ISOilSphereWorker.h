#pragma once
#ifndef _ISOILSPHEREWORKER_H_INCLUDED
#define _ISOILSPHEREWORKER_H_INCLUDED
//-----------------------------------------------------------------------------
#include "ISTcpWorker.h"
//-----------------------------------------------------------------------------
class ISOilSphereWorker : public ISTcpWorker
{
	Q_OBJECT

public:
	Q_INVOKABLE ISOilSphereWorker(QObject *parent);
	virtual ~ISOilSphereWorker();
};
//-----------------------------------------------------------------------------
#endif
