#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISObjectInterface.h"
//-----------------------------------------------------------------------------
class ISAlpha : public ISObjectInterface
{
	Q_OBJECT

public:
	Q_INVOKABLE ISAlpha(QObject *parent);
	virtual ~ISAlpha();

	void RegisterMetaTypes() const override;
	void BeforeShowMainWindow() const override;
	void InitializePlugin() const override;
};
//-----------------------------------------------------------------------------
