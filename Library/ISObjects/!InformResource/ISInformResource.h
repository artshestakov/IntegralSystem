#pragma once
//-----------------------------------------------------------------------------
#include "isobjects_global.h"
#include "ISObjectInterface.h"
//-----------------------------------------------------------------------------
class ISOBJECTS_EXPORT ISInformResource : public ISObjectInterface
{
	Q_OBJECT

public:
	Q_INVOKABLE ISInformResource(QObject *parent);
	virtual ~ISInformResource();

	void RegisterMetaTypes() const override;
	void BeforeShowMainWindow() const override;
	void InitializePlugin() const override;
};
//-----------------------------------------------------------------------------
