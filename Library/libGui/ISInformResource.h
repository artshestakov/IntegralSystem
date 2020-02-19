#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISObjectInterface.h"
//-----------------------------------------------------------------------------
class ISInformResource : public ISObjectInterface
{
	Q_OBJECT

public:
	Q_INVOKABLE ISInformResource();
	virtual ~ISInformResource();

	void RegisterMetaTypes() const override;
	void BeforeShowMainWindow() const override;
	void InitializePlugin() const override;
};
//-----------------------------------------------------------------------------
