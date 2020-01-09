#pragma once
//-----------------------------------------------------------------------------
#include "libgui_global.h"
#include "ISObjectInterface.h"
//-----------------------------------------------------------------------------
class LIBGUI_EXPORT ISInformResource : public ISObjectInterface
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
