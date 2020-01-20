#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISObjectInterface.h"
//-----------------------------------------------------------------------------
class ISDemo : public ISObjectInterface
{
	Q_OBJECT

public:
	Q_INVOKABLE ISDemo(QObject *parent);
	virtual ~ISDemo();

	void RegisterMetaTypes() const override;
	void BeforeShowMainWindow() const override;
	void InitializePlugin() const override;
};
//-----------------------------------------------------------------------------
