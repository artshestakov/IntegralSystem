#pragma once
//-----------------------------------------------------------------------------
#include "isobjects_global.h"
#include "ISObjectInterface.h"
//-----------------------------------------------------------------------------
class ISOBJECTS_EXPORT ISSirona : public ISObjectInterface
{
	Q_OBJECT

public:
	Q_INVOKABLE ISSirona(QObject *parent);
	virtual ~ISSirona();

	void RegisterMetaTypes() const override;
	void BeforeShowMainWindow() const override;
	void InitializePlugin() const override;
};
//-----------------------------------------------------------------------------
