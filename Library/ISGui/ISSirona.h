#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISObjectInterface.h"
//-----------------------------------------------------------------------------
class ISSirona : public ISObjectInterface
{
	Q_OBJECT

public:
	Q_INVOKABLE ISSirona();
	virtual ~ISSirona();

	void RegisterMetaTypes() const override;
	void BeforeShowMainWindow() const override;
	void InitializePlugin() const override;
};
//-----------------------------------------------------------------------------
