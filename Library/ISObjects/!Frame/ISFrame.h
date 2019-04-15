#pragma once
//-----------------------------------------------------------------------------
#include "isobjects_global.h"
#include "ISObjectInterface.h"
//-----------------------------------------------------------------------------
class ISOBJECTS_EXPORT ISFrame : public ISObjectInterface
{
	Q_OBJECT

public:
	Q_INVOKABLE ISFrame(QObject *parent);
	virtual ~ISFrame();

	void RegisterMetaTypes() const override;
	void BeforeShowMainWindow() const override;
	void InitializePlugin() const override;
};
//-----------------------------------------------------------------------------
