#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISObjectInterface.h"
//-----------------------------------------------------------------------------
class ISMedTech : public ISObjectInterface
{
	Q_OBJECT

public:
	Q_INVOKABLE ISMedTech(QObject *parent);
	virtual ~ISMedTech();

	void RegisterMetaTypes() const override;
	void BeforeShowMainWindow() const override;
	void InitializePlugin() const override;
};
//-----------------------------------------------------------------------------
