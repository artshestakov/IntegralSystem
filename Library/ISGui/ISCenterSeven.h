#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISObjectInterface.h"
//-----------------------------------------------------------------------------
class ISCenterSeven : public ISObjectInterface
{
	Q_OBJECT

public:
	Q_INVOKABLE ISCenterSeven();
	virtual ~ISCenterSeven();

	void RegisterMetaTypes() const override;
	void BeforeShowMainWindow() const override;
	void InitializePlugin() const override;
};
//-----------------------------------------------------------------------------
