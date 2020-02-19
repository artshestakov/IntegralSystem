#pragma once
#ifndef _ISMETAUSER_H_INCLUDED
#define _ISMETAUSER_H_INCLUDED
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISObjectInterface.h"
//-----------------------------------------------------------------------------
class ISOilSphere : public ISObjectInterface
{
	Q_OBJECT

public:
	Q_INVOKABLE ISOilSphere();
	virtual ~ISOilSphere();

	void RegisterMetaTypes() const override;
	void BeforeShowMainWindow() const override;
	void InitializePlugin() const override;
};
//-----------------------------------------------------------------------------
#endif
