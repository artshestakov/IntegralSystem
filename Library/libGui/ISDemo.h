#pragma once
//-----------------------------------------------------------------------------
#include "libgui_global.h"
#include "ISObjectInterface.h"
//-----------------------------------------------------------------------------
class LIBGUI_EXPORT ISDemo : public ISObjectInterface
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
