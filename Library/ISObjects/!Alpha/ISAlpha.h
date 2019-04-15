#pragma once
//-----------------------------------------------------------------------------
#include "isobjects_global.h"
#include "ISObjectInterface.h"
//-----------------------------------------------------------------------------
class ISOBJECTS_EXPORT ISAlpha : public ISObjectInterface
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
